extern TCHAR XCSoar_Version[256];

#if !defined(AFX_EXTERNS_H__695AAC30_F401_4CFF_9BD9_FE62A2A2D0D2__INCLUDED_)
#define AFX_EXTERNS_H__695AAC30_F401_4CFF_9BD9_FE62A2A2D0D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sizes.h"
#include "XCSoar.h"
#include "Parser.h"
#include "Calculations.h"
#include "Mapwindow.h"
#include "Task.h"
#include "Statistics.h"
#include "Dialogs.h"

#if (EXPERIMENTAL > 0)
#include "BlueSMS.h"
#endif

// instance of main program
extern HINSTANCE hInst;

// comms data
extern HANDLE hPort1;            // Serial port handle
extern HANDLE hPort2;            // Serial port handle
extern HANDLE hReadThread;      // Handle to the read thread
extern LPTSTR lpszDevName;      // Communication port name
extern BOOL                                    Port1Available;
extern BOOL                                    Port2Available;

// asset/registration data
extern TCHAR strAssetNumber[];
extern TCHAR strRegKey[];

// windows
extern HWND hWndMainWindow;           // HWND Main Window
extern HWND hWndMapWindow;            // HWND MapWindow
extern HWND hWndCB;

// infoboxes
extern int  CurrentInfoType;          // Used for Popup Menu Select
extern int  InfoType[MAXINFOWINDOWS]; //
extern HWND hWndInfoWindow[MAXINFOWINDOWS];
extern int  InfoFocus;
extern bool DisplayLocked; // if infoboxes are locked
extern SCREEN_INFO Data_Options[];
extern int NUMSELECTSTRINGS;
extern BOOL InfoBoxesHidden;
extern int numInfoWindows;

// waypoint data
extern int HomeWaypoint;
extern WAYPOINT *WayPointList;
extern unsigned int NumberOfWayPoints;

// airspace data
extern AIRSPACE_AREA *AirspaceArea;
extern AIRSPACE_POINT *AirspacePoint;
extern AIRSPACE_CIRCLE *AirspaceCircle;
extern unsigned int NumberOfAirspacePoints;
extern unsigned int NumberOfAirspaceAreas;
extern unsigned int NumberOfAirspaceCircles;

// task data
extern TASK_POINT Task[];
extern TASKSTATS_POINT TaskStats[];
extern int ActiveWayPoint;
extern bool TaskAborted;
extern int SelectedWaypoint;
extern int FAISector;
extern DWORD SectorRadius;
extern int StartLine;
extern DWORD StartRadius;
extern double AATTaskLength;
extern BOOL AATEnabled;

// master flight data
extern NMEA_INFO GPS_INFO;
extern DERIVED_INFO CALCULATED_INFO;

// gps detection
extern BOOL GPSCONNECT;
extern BOOL VARIOCONNECT;

// units
extern double SPEEDMODIFY;
extern double LIFTMODIFY;
extern double DISTANCEMODIFY;
extern double ALTITUDEMODIFY;

// polar info
extern double BUGS;
extern double BALLAST;
extern int POLARID;
extern double POLAR[POLARSIZE];
extern double WEIGHTS[POLARSIZE];

extern bool InfoWindowActive;
extern int iAirspaceBrush[];
extern int iAirspaceColour[];

// user interface triggers
extern BOOL TopWindow;
extern bool MapDirty;
extern bool RequestMapDirty;

// snail trail
extern SNAIL_POINT SnailTrail[TRAILSIZE];
extern	int SnailNext;
extern int TrailLock;

// user controls/parameters
extern double MACCREADY;
extern bool   AutoMacCready;
extern double AccelerometerZero;
extern double SAFETYALTITUDEARRIVAL;
extern double SAFETYALTITUDEBREAKOFF;
extern double SAFETYALTITUDETERRAIN;
extern double SAFTEYSPEED;
extern bool LoggerActive;
extern int WindUpdateMode; // unused
extern double QNH;
extern int NettoSpeed;
extern bool EnableCalibration;
extern bool EnableAutoBlank;
extern bool EnableAuxiliaryInfo;
extern int debounceTimeout;

// user interface options
extern bool bAirspaceBlackOutline;
extern int TrailActive;
extern bool CircleZoom;
extern bool EnableTopology;
extern bool EnableTerrain;
extern bool FinalGlideTerrain;
extern bool EnableSoundVario;
extern bool EnableSoundTask;
extern bool EnableSoundModes;
extern int SoundVolume;
extern int SoundDeadband;
extern int DisplayOrientation;
extern int DisplayTextType;
extern int AIRSPACEWARNINGS;
extern int WarningTime;
extern int AcknowledgementTime;
extern int AltitudeMode;
extern int ClipAltitude;
extern int AltWarningMargin;
extern bool EnableCDICruise;
extern bool EnableCDICircling;
extern bool EnableVarioGauge;
//

// statistics
extern Statistics flightstats;

// used in settings dialog
extern BOOL COMPORTCHANGED;
extern BOOL AIRSPACEFILECHANGED;
extern BOOL WAYPOINTFILECHANGED;
extern BOOL TERRAINFILECHANGED;
extern BOOL AIRFIELDFILECHANGED;
extern BOOL TOPOLOGYFILECHANGED;
extern BOOL POLARFILECHANGED;
extern BOOL LANGUAGEFILECHANGED;
extern BOOL STATUSFILECHANGED;
extern BOOL INPUTFILECHANGED;

bool Debounce();


// Interface Globals
extern GetTextSTRUCT GetTextData[];
extern int GetTextData_Size;
extern StatusMessageSTRUCT StatusMessageData[];
extern int StatusMessageData_Size;

#if (EXPERIMENTAL > 0)
extern BlueDialupSMS bsms;
#endif

#endif
