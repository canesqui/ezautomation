

typedef struct {
  int Day;
  int Month;
  int Year;
  int WeekDay;
  int Hour;
  int Minute;
} CurrentDateTime;

typedef struct {
  int Hour;
  int Minute;
  bool State;
  int Relay;
  int WeekDays[7];
} WeeklySchedule;

typedef struct {
  int Hour;
  int Minute;
  bool State;
  int Relay;
  int Day;
  int Month;
  int Year;
} YearlySchedule;




