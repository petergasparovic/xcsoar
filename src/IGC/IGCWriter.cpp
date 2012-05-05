/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "IGC/IGCWriter.hpp"
#include "IO/TextWriter.hpp"
#include "NMEA/Info.hpp"
#include "Version.hpp"
#include "Compatibility/string.h"

#include <assert.h>

#ifdef _UNICODE
#include <windows.h>
#endif

static char *
FormatIGCLocation(char *buffer, const GeoPoint &location)
{
  char latitude_suffix = negative(location.latitude.Native())
    ? 'S' : 'N';
  unsigned latitude =
    (unsigned)uround(fabs(location.latitude.Degrees() * 60000));

  char longitude_suffix = negative(location.longitude.Native())
    ? 'W' : 'E';
  unsigned longitude =
    (unsigned)uround(fabs(location.longitude.Degrees() * 60000));

  sprintf(buffer, "%02u%05u%c%03u%05u%c",
          latitude / 60000, latitude % 60000, latitude_suffix,
          longitude / 60000, longitude % 60000, longitude_suffix);

  return buffer + strlen(buffer);
}

IGCWriter::IGCWriter(const TCHAR *_path, const NMEAInfo &gps_info)
  :simulator(gps_info.alive && !gps_info.gps.real)
{
  _tcscpy(path, _path);

  frecord.Reset();
  last_valid_point_initialized = false;

  if (!simulator)
    grecord.Initialize();
}

bool
IGCWriter::Flush()
{
  if (buffer.IsEmpty())
    return true;

  TextWriter writer(path, true);
  if (writer.error())
    return false;

  for (unsigned i = 0; i < buffer.Length(); ++i) {
    if (!writer.writeln(buffer[i]))
      return false;

    grecord.AppendRecordToBuffer(buffer[i]);
  }

  if (!writer.flush())
    return false;

  buffer.Clear();
  return true;
}

void
IGCWriter::Finish(const NMEAInfo &gps_info)
{
  if (gps_info.alive && !gps_info.gps.real)
    simulator = true;

  Flush();
}

static void
ReplaceNonIGCChars(char *p)
{
  for (; *p != 0; ++p)
    if (!GRecord::IsValidIGCChar(*p))
      *p = ' ';
}

bool
IGCWriter::WriteLine(const char *line)
{
  assert(strchr(line, '\r') == NULL);
  assert(strchr(line, '\n') == NULL);

  if (buffer.IsFull() && !Flush())
    return false;

  assert(!buffer.IsFull());

  char *dest = buffer.Append();
  strncpy(dest, line, MAX_IGC_BUFF);
  dest[MAX_IGC_BUFF - 1] = '\0';

  ReplaceNonIGCChars(dest);

  return true;
}

bool
IGCWriter::WriteLine(const char *a, const TCHAR *b)
{
  size_t a_length = strlen(a);
  size_t b_length = _tcslen(b);
  char buffer[a_length + b_length * 4 + 1];
  memcpy(buffer, a, a_length);

#ifdef _UNICODE
  if (b_length > 0) {
    int len = ::WideCharToMultiByte(CP_ACP, 0, b, b_length,
                                    buffer + a_length, b_length * 4,
                                    NULL, NULL);
    if (len <= 0)
      return false;

    a_length += len;
  }

  buffer[a_length] = 0;
#else
  memcpy(buffer + a_length, b, b_length + 1);
#endif

  return WriteLine(buffer);
}

void
IGCWriter::WriteHeader(const BrokenDateTime &date_time,
                       const TCHAR *pilot_name, const TCHAR *aircraft_model,
                       const TCHAR *aircraft_registration,
                       const TCHAR *competition_id,
                       const char *logger_id, const TCHAR *driver_name)
{
  /*
   * HFDTE141203  <- should be UTC, same as time in filename
   * HFFXA100
   * HFPLTPILOT:JOHN WHARINGTON
   * HFGTYGLIDERTYPE:LS 3
   * HFGIDGLIDERID:VH-WUE
   * HFDTM100GPSDATUM:WGS84
   * HFRFWFIRMWAREVERSION:3.6
   * HFRHWHARDWAREVERSION:3.4
   * HFFTYFR TYPE:GARRECHT INGENIEURGESELLSCHAFT,VOLKSLOGGER 1.0
   * HFCIDCOMPETITIONID:WUE
   * HFCCLCOMPETITIONCLASS:FAI
   */

  assert(date_time.Plausible());
  assert(logger_id != NULL);
  assert(strlen(logger_id) == 3);

  char buffer[100];

  // Flight recorder ID number MUST go first..
  sprintf(buffer, "AXCS%s", logger_id);
  WriteLine(buffer);

  sprintf(buffer, "HFDTE%02u%02u%02u",
          date_time.day, date_time.month, date_time.year % 100);
  WriteLine(buffer);

  if (!simulator)
    WriteLine(GetHFFXARecord());

  WriteLine("HFPLTPILOT:", pilot_name);
  WriteLine("HFGTYGLIDERTYPE:", aircraft_model);
  WriteLine("HFGIDGLIDERID:", aircraft_registration);
  WriteLine("HFCIDCOMPETITIONID:", competition_id);
  WriteLine("HFFTYFRTYPE:XCSOAR,XCSOAR ", XCSoar_VersionStringOld);
  WriteLine("HFGPS:", driver_name);

  WriteLine("HFDTM100DATUM:WGS-84");

  if (!simulator)
    WriteLine(GetIRecord());
}

void
IGCWriter::StartDeclaration(const BrokenDateTime &date_time,
                            const int number_of_turnpoints)
{
  assert(date_time.Plausible());

  // IGC GNSS specification 3.6.1
  char buffer[100];
  sprintf(buffer, "C%02u%02u%02u%02u%02u%02u0000000000%02d",
          // DD  MM  YY  HH  MM  SS  DD  MM  YY IIII TT
          date_time.day,
          date_time.month,
          date_time.year % 100,
          date_time.hour,
          date_time.minute,
          date_time.second,
          number_of_turnpoints - 2);

  WriteLine(buffer);

  // takeoff line
  // IGC GNSS specification 3.6.3
  WriteLine("C0000000N00000000ETAKEOFF");
}

void
IGCWriter::EndDeclaration()
{
  // TODO bug: this is causing problems with some analysis software
  // maybe it's because the date and location fields are bogus
  WriteLine("C0000000N00000000ELANDING");
}

void
IGCWriter::AddDeclaration(const GeoPoint &location, const TCHAR *id)
{
  char c_record[500];
  char id_string[MAX_PATH];
  int i;

  TCHAR tmpstring[MAX_PATH];
  _tcscpy(tmpstring, id);
  _tcsupr(tmpstring);
  for (i = 0; i < (int)_tcslen(tmpstring); i++)
    id_string[i] = (char)tmpstring[i];

  id_string[i] = '\0';

  char *p = c_record;
  *p++ = 'C';
  p = FormatIGCLocation(p, location);
  strcpy(p, id_string);

  WriteLine(c_record);
}

void
IGCWriter::LoggerNote(const TCHAR *text)
{
  WriteLine("LPLT", text);
}

/**
 * Applies range checks to the specified altitude value and converts
 * it to an integer suitable for printing in the IGC file.
 */
static int
NormalizeIGCAltitude(int value)
{
  if (value < -9999)
    /* for negative values, there are only 4 characters left (after
       the minus sign), and besides that, IGC does not support a
       journey towards the center of the earth */
    return -9999;

  if (value >= 99999)
    /* hooray, new world record! .. or just some invalid value; we
       have only 5 characters for the altitude, so we must clip it at
       99999 */
    return 99999;

  return value;
}

void
IGCWriter::LogPoint(const IGCFix &fix, int epe, int satellites)
{
  char b_record[500];
  char *p = b_record;

  sprintf(p, "B%02d%02d%02d", fix.time.hour, fix.time.minute, fix.time.second);
  p += strlen(p);

  p = FormatIGCLocation(p, fix.location);

  sprintf(p, "%c%05d%05d%03d%02d",
          fix.gps_valid ? 'A' : 'V',
          NormalizeIGCAltitude(fix.pressure_altitude),
          NormalizeIGCAltitude(fix.gps_altitude),
          epe, satellites);

  WriteLine(b_record);
}

void
IGCWriter::LogPoint(const NMEAInfo& gps_info)
{
  int satellites = GetSIU(gps_info);
  fixed epe = GetEPE(gps_info);
  IGCFix fix;

  // if at least one GPS fix comes from the simulator, disable signing
  if (gps_info.alive && !gps_info.gps.real)
    simulator = true;

  if (!simulator) {
    const char *f_record = frecord.Update(gps_info.gps, gps_info.date_time_utc,
                                          gps_info.time,
                                          !gps_info.location_available);
    if (f_record != NULL)
      WriteLine(f_record);
  }

  if (!last_valid_point_initialized &&
      ((gps_info.gps_altitude < fixed(-100))
       || (gps_info.baro_altitude < fixed(-100))
          || !gps_info.location_available))
    return;


  if (!gps_info.location_available) {
    fix = last_valid_point;
    fix.gps_valid = false;
  } else {
    fix.gps_valid = true;
    fix.location = gps_info.location;
    fix.gps_altitude = (int)gps_info.gps_altitude;

    // save last active fix location
    last_valid_point = fix;
    last_valid_point_initialized = true;
  }

  fix.time = gps_info.date_time_utc;
  fix.pressure_altitude =
      gps_info.baro_altitude_available ? (int)gps_info.baro_altitude :
                                         /* fall back to GPS altitude */
                                         fix.gps_altitude;

  LogPoint(fix, (int)epe, satellites);
}

void
IGCWriter::LogEvent(const NMEAInfo &gps_info, const char *event)
{
  char e_record[30];
  sprintf(e_record,"E%02d%02d%02d%s",
          gps_info.date_time_utc.hour, gps_info.date_time_utc.minute,
          gps_info.date_time_utc.second, event);

  WriteLine(e_record);
  // tech_spec_gnss.pdf says we need a B record immediately after an E record
  LogPoint(gps_info);
}

void
IGCWriter::Sign()
{
  if (simulator)
    return;

  // buffer is appended w/ each igc file write
  grecord.FinalizeBuffer();
  // read record built by individual file writes
  char OldGRecordBuff[MAX_IGC_BUFF];
  grecord.GetDigest(OldGRecordBuff);

  // now calc from whats in the igc file on disk
  grecord.Initialize();
  grecord.SetFileName(path);
  grecord.LoadFileToBuffer();
  grecord.FinalizeBuffer();
  char NewGRecordBuff[MAX_IGC_BUFF];
  grecord.GetDigest(NewGRecordBuff);

  bool bFileValid = strcmp(OldGRecordBuff, NewGRecordBuff) == 0;
  grecord.AppendGRecordToFile(bFileValid);
}
