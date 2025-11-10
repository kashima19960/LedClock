#include "app_state.h"

const uint16_t tempertureMap[] = {
    1054, 1091, 1128, 1165, 1203, 1242, 1280, 1320, 1359, 1399, 1439, 1479, 1520, 1560, 1601, 1642, 1683, 1724,
    1765, 1805, 1846, 1887, 1927, 1968, 2008, 2048, 2087, 2126, 2165, 2204, 2242, 2280, 2318, 2355, 2391, 2427,
    2463, 2498, 2533, 2567, 2601, 2634, 2667, 2699, 2730, 2761, 2791, 2821, 2850, 2879, 2907, 2935, 2962, 2988,
    3014, 3040, 3064, 3089, 3113, 3136, 3158, 3181, 3202, 3224, 3244, 3264, 3284, 3303, 3322, 3341, 3359, 3376,
    3393, 3410, 3426, 3442, 3457, 3472, 3487, 3501, 3515, 3529, 3542, 3555, 3568, 3580, 3592, 3603, 3615, 3626,
    3637, 3647, 3657, 3667, 3677, 3687, 3696, 3705, 3714, 3722, 3730, 3739, 3747, 3754, 3762, 3769, 3776, 3783,
    3790, 3797, 3803, 3809, 3815, 3821, 3827, 3833, 3839, 3844, 3849, 3854, 3859, 3864, 3869, 3874, 3878, 3883,
};

DisplayMode currentMode = MODE_SHOW_TIME;
bool isInitCompleted = false;
uint32_t lastDisplayChangeTime;

uint32_t adcValue[2];

uint8_t savedBrightness = 0;
bool isWeakBrightness = true;
uint8_t strongBrightness, weakBrightness;

DateTime time, lastTime;
uint8_t blinkControl;

bool isAlarmEnabled = false;
bool isAlarmed = false, isAlarming = false;
uint8_t alarmHour = 0, alarmMin = 0;

bool isRingOnTimeEnabled;
uint8_t ringOnTimeStart, ringOnTimeStop;
uint8_t lastRingOnTimeHour;
uint32_t ringStartTime;

uint32_t lastModeKeyPressTime, lastSetKeyPressTime, lastSetKeyPressReportTime;
uint32_t alarmTimestamp, alarmBeepCount;
bool setKeyRepeatReported = false;

uint8_t temperture = 25;
uint8_t tempertureShowTime, tempertureHideTime;
uint8_t tempBuffered = 0;
uint16_t tempBuffer[TEMP_BUFFER_SIZE + 1];
