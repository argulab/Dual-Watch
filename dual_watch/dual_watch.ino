#include <Wire.h>                                                                                                                             #include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <WString.h>

#define pin_button_clock_mode 11
#define pin_button_clock_up 9
#define pin_button_clock_down 15

//#define LANGUAGE_ENGLISH
#define LANGUAGE_SPANISH
//#define LANGUAGE_EUSKERA

#ifdef LANGUAGE_SPANISH
  #define week_1 "LUNES"
  #define week_2 "MARTES"
  #define week_3 "MIERCOLES"
  #define week_4 "JUEVES"
  #define week_5 "VIERNES"
  #define week_6 "SABADO"
  #define week_7 "DOMINGO"
  #define month_01 "ENERO"
  #define month_02 "FEBRERO"
  #define month_03 "MARZO"
  #define month_04 "ABRIL"
  #define month_05 "MAYO"
  #define month_06 "JUNIO"
  #define month_07 "JULIO"
  #define month_08 "AGOSTO"
  #define month_09 "SEPTIEMBRE"
  #define month_10 "OCTUBRE"
  #define month_11 "NOVIEMBRE"
  #define month_12 "DICIEMBRE"
#endif //LANGUAGE_SPANISH

#ifdef LANGUAGE_ENGLISH
  #define week_1 "MONDAY"
  #define week_2 "TUESDAY"
  #define week_3 "WEDNESDAY"
  #define week_4 "THURSDAY"
  #define week_5 "FRIDAY"
  #define week_6 "SATURDAY"
  #define week_7 "SUNDAY"
  #define month_01 "JANUARY"
  #define month_02 "FEBRUARY"
  #define month_03 "MARS"
  #define month_04 "APRIL"
  #define month_05 "MAY"
  #define month_06 "JUNE"
  #define month_07 "JULY"
  #define month_08 "AUGUST"
  #define month_09 "SEPTEMBER"
  #define month_10 "OCTOBER"
  #define month_11 "NOVEMBER"
  #define month_12 "DECEMBER"
#endif //LANGUAGE_ENGLISH

#ifdef LANGUAGE_EUSKERA
  #define week_1 "ASTELENA"
  #define week_2 "ASTEARTEA"
  #define week_3 "ASTEASKENA"
  #define week_4 "OSTEGUNA"
  #define week_5 "OSTIRALA"
  #define week_6 "LARUNBATA"
  #define week_7 "IGANDEA"
  #define month_01 "URTARRILA"
  #define month_02 "OTSAILA"
  #define month_03 "MARTXOA"
  #define month_04 "APIRILA"
  #define month_05 "MAIATZA"
  #define month_06 "EKAINA"
  #define month_07 "UTZAILA"
  #define month_08 "ABUZTUA"
  #define month_09 "IRAILA"
  #define month_10 "URRIA"
  #define month_11 "AZAROA"
  #define month_12 "ABENDUA"
#endif //LANGUAGE_EUSKERA

enum month_name {january = 1, february, march, april, may, june, july, august, september, october, november, december};
enum dow_name {sunday, monday, tuesday, wednesday, thusday, friday, saturday};
enum clock_mode {mode_normal, mode_year, mode_month, mode_day, mode_hour, mode_minute, mode_end_of_process};

month_name mn_month;
dow_name dn_dow;
clock_mode cm_mode = mode_normal;

LiquidCrystal_I2C lcd(0x27, 20, 4);
RTC_DS3231 rtc;

String name_of_month(month_name m) {
  switch (m) {
    case january:   return month_01;
    case february:  return month_02;
    case march:     return month_03;
    case april:     return month_04;
    case may:       return month_05;
    case june:      return month_06;
    case july:      return month_07;
    case august:    return month_08;
    case september: return month_09;
    case october:   return month_10;
    case november:  return month_11;
    case december:  return month_12;
   }
  }

String name_of_the_day_of_the_week(dow_name d) {
  switch (d) {
    case sunday:    return week_7;
    case monday:    return week_1;
    case tuesday:   return week_2;
    case wednesday: return week_3;
    case thusday:   return week_4;
    case friday:    return week_5;
    case saturday:  return week_6;
  }
}

int calc_time_offset(DateTime dt_utc) { 
  byte last_day_before_last_monday = 24;  
  byte hour_of_change = 2;
  int winter_offset = 3600;     //1 hour = 3600sec
  int summer_offset = 3600 * 2; //2 hour
  month_name number_month = dt_utc.month();
  dow_name dow_number = dt_utc.dayOfTheWeek();

  if (number_month > march && number_month < october) {
    return summer_offset;
  }

  if (number_month > october || number_month < march) {
    return winter_offset;
  }

  if (number_month == march) {
    if (dt_utc.day() < last_day_before_last_monday) {
      return winter_offset;    
    } else {
      if (dow_number == sunday) {
        if (dt_utc.hour() < hour_of_change) {
          return winter_offset;
        } else {
          return summer_offset;
        }
      } else {
        if (last_day_before_last_monday - dt_utc.day() + dow_number >= 0) {
          return winter_offset;
        } else {
          return summer_offset;
        }
      }
    }
  }

  if (number_month = october) {
    if (dt_utc.day() < last_day_before_last_monday) {
      return summer_offset;    
    } else {
      if (dow_number == sunday) {
        if (dt_utc.hour() < hour_of_change) {
          return summer_offset;
        } else {
          return winter_offset;
        }
      } else {
        if (last_day_before_last_monday - dt_utc.day() + dow_number >= 0) {
          return summer_offset;
        } else {
          return winter_offset;
        }
      }
    }   
  }
}

byte check_next_day() {
  DateTime dt_utc = rtc.now();
  int time_offset = calc_time_offset(dt_utc);
  DateTime dt_local = dt_utc.unixtime() + time_offset;
  bool is_next_day = dt_local.hour() == 0 && dt_local.minute() == 0 && dt_local.second() == 0;

  if (is_next_day) {
    delay(1000);
  }
  return is_next_day;
}

void show_invalid_data() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  DEBE INTRODUCIR");
  lcd.setCursor(0, 1);
  lcd.print("  UNA FECHA VALIDA");
  lcd.setCursor(0, 2);
  lcd.print("        Y/O");
  lcd.setCursor(0, 3);
  lcd.print("    HORA VALIDOS");
  delay(2000);
  lcd.clear();  
}

bool button_pressed (int button) {
  bool bp = digitalRead(button) == LOW;
  if (bp) {
    delay(250); 
  }
  return bp;
}

void show_end_of_process_display () {
  lcd.setCursor(0, 0);
  lcd.print("B.SUP(+)  = GRABAR ");
  
  lcd.setCursor(0, 1);
  lcd.print("B.INF(-) = SUSPENDER"); 
}

void switch_clock_mode() {
  if (cm_mode == mode_end_of_process) {
    cm_mode = mode_normal;
  } else {
    cm_mode = cm_mode + 1;
  }
}

void clear_clock_display() {
  lcd.setCursor(0, 0);
  lcd.print("                    ");
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  lcd.setCursor(0, 2);
  lcd.print("                    "); 
}

void show_settings_label (String s_label) {
  lcd.setCursor(0, 0);
  lcd.print("       ");
  lcd.setCursor(0, 0);
  lcd.print(s_label);  
}

void show_settings_data(int i_data) {
  lcd.setCursor(9, 0);
  lcd.print("    "); 
  lcd.setCursor(9, 0);
  lcd.print(i_data, DEC);  
}


void show_set_date_time(int yr, int mt, int dy, int hr, int me) { 
  lcd.setCursor(0, 2);
  lcd.print("                    ");
  lcd.setCursor(0, 2);
  lcd.print(String(dy) + "/" + String(mt) + "/" + String(yr));
  
  lcd.setCursor(11, 2);
  lcd.print("     ");
  lcd.setCursor(11, 2);
  lcd.print(String(hr) + ":" + String(me));
}

int month_bounds(int mt) {
  if (mt > 12) {
    return 1;
  } else if (mt == 0) {
    return 12;
  } else {
    return mt;
  }
}

int day_bounds(int yr, int mt, int dy) { 
  int day_top = 31;
  
  if (mt == february) {
    if (leap_year(yr)) {
      day_top = 29;
    } else {
      day_top = 28;
    }
  } else if (mt == april || mt == june || mt == september || mt == november) {
    day_top = 30;
  }

  if (dy > day_top) {
    return 1;
  } else if (dy == 0) {
    return day_top;
  } else {
    return dy;
  }
}

int hour_bounds(int hr) {
  if (hr > 23) {
    return 0;
  } else if (hr < 0) {
    return 23;
  } else {
    return hr;
  }
}

int minute_bounds(int mi) {
  if (mi > 59) {
    return 0;
  } else if (mi < 0) {
    return 59;
  } else {
    return mi;
  }
}

bool leap_year (int y) {
  return y%4 == 0 && y%10 != 0;
}

bool check_valid_data(int dy, int mt, int yr, int mi, int se) {
  int days_offset = 0;

  if (mt == april || mt == june || mt == september || mt == november) {
    days_offset = 1;
  } else if (mt == february) {
    if (leap_year(yr)) {
      days_offset = 2;
    } else {
      days_offset = 3;
    }
  }

  if (mt <= january || mt > december) {
    return false;
  } else if (dy <= 0 || dy + days_offset > 31) {
    return false;  
  } else if (mi < 0 || mi > 59) {
    return false;
  } else {
    return true;
  }
}

void service_clock_set_serial () {
  if (Serial.available()) {
    char set_time[14];
    size_t count = Serial.readBytesUntil('\n', set_time, 14);

    String dt = set_time;

    int i_day =    (dt.substring(0, 2)).toInt();
    int i_month =  (dt.substring(2, 4)).toInt();
    int i_year =   (dt.substring(4, 8)).toInt();
    int i_hour =   (dt.substring(8, 10)).toInt();
    int i_minute = (dt.substring(10, 12)).toInt();


    if (check_valid_data(i_day, i_month, i_year, i_hour, i_minute)) {
      rtc.adjust(DateTime(i_year, i_month, i_day, i_hour, i_minute, 0)); 
    } else {
      show_invalid_data();
    }
    process_date();
    process_time();
  } 
}

int read_buttons_updown() {
  if (button_pressed (pin_button_clock_up)) {
    return 1;   
  } else if (button_pressed(pin_button_clock_down)) {
    return -1;
  } else {
    return 0;
  }
} 

void service_set_clock_time () {
  DateTime dt_utc = rtc.now();
  int time_offset = calc_time_offset(dt_utc);
  DateTime dt_local = dt_utc.unixtime() + time_offset;

  int i_year =   dt_local.year();
  int i_month =  dt_local.month();
  int i_day =    dt_local.day();
  int i_hour =   dt_local.hour();
  int i_minute = dt_local.minute();
  int now_mode = mode_year;
  String s_mode = "YEAR";
  int i_dat = dt_local.year();
  int buttons_updown = 0;

  clear_clock_display();
  show_settings_label(s_mode);
  show_settings_data(i_dat);
  show_set_date_time(i_year, i_month, i_day, i_hour, i_minute);
  
  while (cm_mode != mode_normal) {
    if (button_pressed(pin_button_clock_mode)) {
      switch_clock_mode();
    } else {
      buttons_updown = read_buttons_updown ();
    }

    switch(cm_mode) {
      case mode_year:
        i_year += buttons_updown;        
        i_dat = i_year;
        s_mode = "YEAR";
        break;
      case mode_month:
        i_month += buttons_updown;
        i_month = month_bounds(i_month);         
        i_dat = i_month;
        s_mode = "MONTH";
        break;
      case mode_day: 
        i_day += buttons_updown;  
        i_day = day_bounds(i_year, i_month, i_day);         
        i_dat = i_day;
        s_mode = "DAY";
        break;
      case mode_hour:     
        i_hour = hour_bounds(i_hour += buttons_updown); 
        i_dat = i_hour;
        s_mode = "HOUR";        
        break;
      case mode_minute:       
        i_minute = minute_bounds(i_minute += buttons_updown); 
        i_dat = i_minute;
        s_mode = "MINUTE";
        break;
      case mode_end_of_process:
        show_end_of_process_display ();
        if (buttons_updown != 0) {
          if (buttons_updown == 1) { 
            rtc.adjust(DateTime(i_year, i_month, i_day, i_hour, i_minute, 0));
          }
          cm_mode = mode_normal;
        }
        break;
    }    
    if (now_mode != cm_mode) {
      show_settings_label(s_mode);
      show_settings_data(i_dat);
      now_mode = cm_mode;
    }

    if (buttons_updown != 0) {
      show_settings_data(i_dat);
      show_set_date_time(i_year, i_month, i_day, i_hour, i_minute);
    }
  }
  clear_clock_display();
  process_date();
}

void process_date() {
  DateTime dt_utc = rtc.now();
  int time_offset = calc_time_offset(dt_utc);
  DateTime dt_local = dt_utc.unixtime() + time_offset; 
  byte col_max = 20;
  byte col_init = 0;
  byte row_date = 0;
  byte row_day_of_the_week = 1;
  String s_month = name_of_month(dt_local.month());
  String s_day_of_the_week = name_of_the_day_of_the_week(dt_local.dayOfTheWeek());
  String s_date = String(dt_local.day());

  s_date.concat(" * ");
  s_date.concat(s_month);
  s_date.concat(" * " );
  s_date.concat(dt_local.year());
 
  lcd.setCursor(col_init, row_date);
  lcd.print("                    ");
  lcd.setCursor(((col_max - s_date.length()) / 2), row_date);
  lcd.print(s_date);

  lcd.setCursor(col_init, row_day_of_the_week);
  lcd.print("                    ");
  lcd.setCursor(((col_max - s_day_of_the_week.length()) / 2), row_day_of_the_week);
  lcd.print(s_day_of_the_week); 
}

void process_time() {
  DateTime dt_utc = rtc.now();
  int time_offset = calc_time_offset(dt_utc);
  DateTime dt_local = dt_utc.unixtime() + time_offset;
  char buf_local[] = "hh:mm:ss";
  char buf_utc[] = "hh:mm";
  byte col_local = 0;
  byte col_utc = 11;
  byte row_hour = 2;
  
  lcd.setCursor(col_local, row_hour);
  lcd.print(dt_local.toString(buf_local));

  lcd.setCursor(col_utc, row_hour);
  lcd.print("UTC=");
  lcd.print(dt_utc.toString(buf_utc));
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  rtc.begin();

  lcd.init();
  lcd.backlight();
  lcd.clear();
    
  pinMode(pin_button_clock_up, INPUT_PULLUP);
  pinMode(pin_button_clock_mode, INPUT_PULLUP);
  pinMode(pin_button_clock_down, INPUT_PULLUP);
  
  process_date();
}

void loop() {
  service_clock_set_serial();
  if (button_pressed(pin_button_clock_mode) && cm_mode == mode_normal) {
    cm_mode = mode_year;
    service_set_clock_time();
  }

  if (cm_mode == mode_normal) {
    if (check_next_day()) {
      process_date();
    }
    process_time();
  }
}
