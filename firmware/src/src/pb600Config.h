// =======================================================================================================
// PB600 SOUND: Einstellungen über USB (Web Serial, Seite "PB600 Sound Setup")
// =======================================================================================================
//
// Zeilenprotokoll, 115200 Baud. Alle Antworten beginnen mit "PB:", andere Ausgaben ignoriert die Seite.
//   ?            -> PB:CFG {json}          aktuelle Werte, Grenzen, Standardwerte
//   S key wert   -> PB:OK key wert         Wert setzen und sofort anwenden (noch nicht gespeichert)
//   W            -> PB:SAVED               im ESP32 speichern (bleibt nach dem Ausschalten)
//   R            -> PB:CFG {json}          gespeicherte Werte neu laden
//   D            -> PB:CFG {json}          Werkseinstellungen anwenden (erst mit W gespeichert)
//   C F / C R    -> PB:CFG {json}          aktuellen Gas-Ausschlag als Endpunkt vorwärts / rückwärts übernehmen
//   L1 / L0      -> PB:LIVE {json} alle 100 ms an / aus
//
#pragma once
#ifdef PB600_CONFIG
#include <Preferences.h>
#include <esp_system.h>

#define PB600_FW_VERSION "1.7"

enum { K_MASTER, K_IDLE, K_FULL, K_START, K_STOP, K_HORN, K_BEEP, K_RPM, K_RISE, K_FALL, K_FWD, K_REV, K_COUNT };
static const char *const PB_KEY[K_COUNT] = {"master", "idle", "full", "start", "stop", "horn", "beep", "rpm", "rise", "fall", "fwd", "rev"};
static const int16_t PB_MIN[K_COUNT] = {0, 0, 0, 0, 0, 0, 0, 100, 1, 1, 1600, 1000};
static const int16_t PB_MAX[K_COUNT] = {150, 200, 250, 200, 200, 250, 250, 200, 10, 20, 2000, 1400};
static int16_t pbDef[K_COUNT];
static int16_t pbCfg[K_COUNT];
static Preferences pbPrefs;
static bool pbLive = false;
static bool pbStarted = false;
static bool pbPrefsOk = false;
static char pbLine[48];
static uint8_t pbLen = 0;

static void pbApply()
{
  masterVolume = pbCfg[K_MASTER];
  engineIdleVolumePercentage = pbCfg[K_IDLE];
  fullThrottleVolumePercentage = pbCfg[K_FULL];
  startVolumePercentage = pbCfg[K_START];
  shutdownVolumePercentage = pbCfg[K_STOP];
  hornVolumePercentage = pbCfg[K_HORN];
  reversingVolumePercentage = pbCfg[K_BEEP];
  MAX_RPM_PERCENTAGE = pbCfg[K_RPM];
  minSampleInterval = 4000000 / sampleRate * 100 / MAX_RPM_PERCENTAGE;
  acc = pbCfg[K_RISE];
  dec = pbCfg[K_FALL];
  pulseMax[3] = pbCfg[K_FWD]; // Gas-Endpunkte: Vollgas vorwärts / rückwärts = 100 %
  pulseMin[3] = pbCfg[K_REV];
}

static void pbPrintCfg()
{
  Serial.printf("PB:CFG {\"fw\":\"%s\"", PB600_FW_VERSION);
  for (uint8_t k = 0; k < K_COUNT; k++)
    Serial.printf(",\"%s\":[%d,%d,%d,%d]", PB_KEY[k], pbCfg[k], PB_MIN[k], PB_MAX[k], pbDef[k]);
  Serial.print("}\n");
}

// Gespeichert wird ein einziger Block (Version + alle Werte): ein Schreibvorgang statt 13.
struct PbStore
{
  uint16_t ver;
  int16_t v[K_COUNT];
};
#define PB_STORE_VER 2

// Flash-Zugriffe nur bei angehaltener Sound-Wiedergabe (siehe RCES "disableAllInterrupts")
static void pbAudio(bool on)
{
  if (variableTimer == NULL || fixedTimer == NULL)
    return;
  if (on)
  {
    timerWrite(variableTimer, 0); // Zähler zurück, damit der Alarm sicher wieder auslöst
    timerWrite(fixedTimer, 0);
    timerAlarmEnable(variableTimer);
    timerAlarmEnable(fixedTimer);
  }
  else
  {
    timerAlarmDisable(variableTimer);
    timerAlarmDisable(fixedTimer);
  }
}

static void pbLoad()
{
  for (uint8_t k = 0; k < K_COUNT; k++)
    pbCfg[k] = pbDef[k];
  PbStore st;
  pbAudio(false);
  size_t n = pbPrefs.getBytes("cfg", &st, sizeof(st));
  pbAudio(true);
  if (n == sizeof(st) && st.ver == PB_STORE_VER)
    for (uint8_t k = 0; k < K_COUNT; k++)
      pbCfg[k] = constrain(st.v[k], PB_MIN[k], PB_MAX[k]);
  pbApply();
}

static bool pbSave()
{
  PbStore st;
  st.ver = PB_STORE_VER;
  for (uint8_t k = 0; k < K_COUNT; k++)
    st.v[k] = pbCfg[k];
  pbAudio(false);
  size_t n = pbPrefs.putBytes("cfg", &st, sizeof(st));
  PbStore chk;
  size_t r = pbPrefs.getBytes("cfg", &chk, sizeof(chk));
  pbAudio(true);
  bool ok = n == sizeof(st) && r == sizeof(chk) && !memcmp(&st, &chk, sizeof(st));
  Serial.printf("PB:DBG save put=%u get=%u ok=%d free=%u\n", (unsigned)n, (unsigned)r, ok, (unsigned)pbPrefs.freeEntries());
  return ok;
}

static void pb600ConfigBegin()
{
  // Standardwerte = Werte aus dem Fahrzeugprofil
  pbDef[K_MASTER] = 100;
  pbDef[K_IDLE] = engineIdleVolumePercentage;
  pbDef[K_FULL] = fullThrottleVolumePercentage;
  pbDef[K_START] = startVolumePercentage;
  pbDef[K_STOP] = shutdownVolumePercentage;
  pbDef[K_HORN] = hornVolumePercentage;
  pbDef[K_BEEP] = reversingVolumePercentage;
  pbDef[K_RPM] = MAX_RPM_PERCENTAGE;
  pbDef[K_RISE] = acc;
  pbDef[K_FALL] = dec;
  pbDef[K_FWD] = pulseMax[3];
  pbDef[K_REV] = pulseMin[3];
  pbAudio(false);
  pbPrefsOk = pbPrefs.begin("pb600", false);
  pbAudio(true);
  pbLoad();
  Serial.printf("PB:READY %s %d\n", PB600_FW_VERSION, (int)esp_reset_reason());
}

static void pbCommand(char *c)
{
  while (*c == ' ')
    c++;
  switch (c[0])
  {
  case '?':
    pbPrintCfg();
    break;
  case 'S':
  {
    char key[12];
    int val;
    if (sscanf(c + 1, " %11s %d", key, &val) == 2)
      for (uint8_t k = 0; k < K_COUNT; k++)
        if (!strcmp(key, PB_KEY[k]))
        {
          pbCfg[k] = constrain(val, PB_MIN[k], PB_MAX[k]);
          pbApply();
          Serial.printf("PB:OK %s %d\n", PB_KEY[k], pbCfg[k]);
          return;
        }
    Serial.print("PB:ERR set\n");
    break;
  }
  case 'W':
    Serial.print("PB:SAVING\n");
    Serial.flush();
    if (!pbPrefsOk)
    {
      pbAudio(false);
      pbPrefsOk = pbPrefs.begin("pb600", false);
      pbAudio(true);
    }
    if (pbPrefsOk && pbSave())
      Serial.print("PB:SAVED\n");
    else
      Serial.printf("PB:ERR save %d\n", pbPrefsOk ? 2 : 1);
    break;
  case 'R':
    pbLoad();
    pbPrintCfg();
    break;
  case 'D':
    for (uint8_t k = 0; k < K_COUNT; k++)
      pbCfg[k] = pbDef[k];
    pbApply();
    pbPrintCfg();
    break;
  case 'C':
  {
    char which = 0;
    sscanf(c + 1, " %c", &which);
    int16_t p = pulseWidthRaw3[3];
    if (which == 'F' && p >= PB_MIN[K_FWD] && p <= PB_MAX[K_FWD])
      pbCfg[K_FWD] = p;
    else if (which == 'R' && p >= PB_MIN[K_REV] && p <= PB_MAX[K_REV])
      pbCfg[K_REV] = p;
    else
    {
      Serial.printf("PB:ERR cal %d\n", p);
      break;
    }
    pbApply();
    pbPrintCfg();
    break;
  }
  case 'L':
    pbLive = (c[1] == '1');
    Serial.printf("PB:LIVEMODE %d\n", pbLive);
    break;
  default:
    Serial.print("PB:ERR cmd\n");
  }
}

void pb600ConfigLoop()
{
  if (!pbStarted)
  { // erst im ersten loop(): setup() hat dann alle Werte berechnet
    pbStarted = true;
    pb600ConfigBegin();
  }
  while (Serial.available() > 0)
  {
    char ch = Serial.read();
    if (ch == '\n' || ch == '\r')
    {
      if (pbLen)
      {
        pbLine[pbLen] = 0;
        pbCommand(pbLine);
        pbLen = 0;
      }
    }
    else if (pbLen < sizeof(pbLine) - 1)
      pbLine[pbLen++] = ch;
    else
      pbLen = 0; // Müll verwerfen
  }
  static unsigned long lastLive;
  if (pbLive && millis() - lastLive > 100)
  {
    lastLive = millis();
    Serial.printf("PB:LIVE {\"thr\":%d,\"fn\":%d,\"t\":%d,\"rpm\":%d,\"run\":%d}\n",
                  pulseWidthRaw3[3], pulseWidthRaw3[4], currentThrottle, currentRpm, engineRunning ? 1 : 0);
  }
}
#endif
