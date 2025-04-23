#include "DateTime.hpp"

DateTime::DateTime(){}

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond) {
    // Проверка корректности даты
    auto ymd = std::chrono::year{year} / month / day;
    if (!ymd.ok()) {
        throw std::invalid_argument("Invalid date");
    }

    // Проверка корректности времени
    if (hour < 0 || hour >= 24 || minute < 0 || minute >= 60 || 
        second < 0 || second >= 60 || millisecond < 0 || millisecond >= 1000) {
        throw std::invalid_argument("Invalid time");
    }
    time_point_ = TimePoint(Days(ymd) + 
                            std::chrono::hours(hour) + 
                            std::chrono::minutes(minute) + 
                            std::chrono::seconds(second) + 
                            std::chrono::milliseconds(millisecond));
}

DateTime::DateTime(const std::string& datetimeStr) {
    std::tm tm = {};
    std::istringstream ss(datetimeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        throw std::runtime_error("Неверный формат даты: " + datetimeStr);
    }

    auto pos = datetimeStr.find('.');
    int millis = 0;
    if (pos != std::string::npos) {
        millis = std::stoi(datetimeStr.substr(pos + 1, 3)); // Берём 3 знака после точки
    }

    time_point_ = Clock::from_time_t(timegm(&tm)) + std::chrono::milliseconds(millis);
}

DateTime DateTime::Now() { 
    using namespace std::chrono;
    auto local_time = zoned_time{current_zone(), system_clock::now()}.get_local_time();
    return DateTime((TimePoint)local_time.time_since_epoch());
}

int DateTime::Year(const DateTime& dt)    { return dt.getTm().tm_year + 1900; }
int DateTime::Month(const DateTime& dt)   { return dt.getTm().tm_mon + 1; }
int DateTime::Day(const DateTime& dt)     { return dt.getTm().tm_mday; }
int DateTime::Hour(const DateTime& dt)    { return dt.getTm().tm_hour; }
int DateTime::Minute(const DateTime& dt)  { return dt.getTm().tm_min; }
int DateTime::Second(const DateTime& dt)  { return dt.getTm().tm_sec; }

DateTime DateTime::operator+(int days) const { return DateTime(time_point_ + std::chrono::days(days)); }
DateTime DateTime::operator-(int days) const { return DateTime(time_point_ - std::chrono::days(days)); }
int DateTime::operator-(const DateTime& other) const {
    return std::chrono::duration_cast<std::chrono::days>(time_point_ - other.time_point_).count();
}

bool DateTime::operator==(const DateTime& other) const { return time_point_ == other.time_point_; }
bool DateTime::operator!=(const DateTime& other) const { return !(*this == other); }
bool DateTime::operator<(const DateTime& other) const { return time_point_ < other.time_point_; }
bool DateTime::operator>(const DateTime& other) const { return time_point_ > other.time_point_; }
bool DateTime::operator<=(const DateTime& other) const { return !(*this > other); }
bool DateTime::operator>=(const DateTime& other) const { return !(*this < other); }

DateTime& DateTime::addYears(int years)   { return adjustDate(std::chrono::years(years)); }
DateTime& DateTime::addMonths(int months) { return adjustDate(std::chrono::months(months)); }
DateTime& DateTime::addDays(int days)     { return adjustDate(std::chrono::days(days)); }
DateTime& DateTime::addHours(int h)       { return adjustTime(std::chrono::hours(h)); }
DateTime& DateTime::addMinutes(int m)     { return adjustTime(std::chrono::minutes(m)); }
DateTime& DateTime::addSeconds(int s)     { return adjustTime(std::chrono::seconds(s)); }
DateTime& DateTime::addMilliseconds(int ms) { return adjustTime(std::chrono::milliseconds(ms)); }

std::string DateTime::toString(bool withoutTime, const std::string& format) const{
    TimePoint tp = time_point_;
    static const std::unordered_map<int, std::string> month_short = {
        {1, "янв."}, {2, "фев."}, {3, "мар."}, {4, "апр."}, {5, "май"}, {6, "июн."},
        {7, "июл."}, {8, "авг."}, {9, "сен."}, {10, "окт."}, {11, "нояб."}, {12, "дек."}
    };
    static const std::unordered_map<int, std::string> month_full = {
        {1, "января"}, {2, "февраля"}, {3, "марта"}, {4, "апреля"}, {5, "мая"}, {6, "июня"},
        {7, "июля"}, {8, "августа"}, {9, "сентября"}, {10, "октября"}, {11, "ноября"}, {12, "декабря"}
    };
    
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
        tp.time_since_epoch()) % 1000;
    if (millis.count() < 0) {
        tp -= std::chrono::seconds(1);
        millis += std::chrono::milliseconds(1000);
    }

    std::tm tm = getTm(tp);
    std::ostringstream oss;

    for (size_t i = 0; i < format.size(); ++i) {
        if (format[i] == 'd' && i + 1 < format.size() && format[i + 1] == 'd') {
            oss << std::setw(2) << std::setfill('0') << tm.tm_mday; i+=1; continue;
        } else if (format[i] == 'd') {
            oss << tm.tm_mday; continue;
        } else if (format[i] == 'm' && i + 3 < format.size() && format.substr(i, 4) == "mmmm") {
            oss << month_full.at(tm.tm_mon + 1); i += 3; continue;
        } else if (format[i] == 'm' && i + 2 < format.size() && format.substr(i, 3) == "mmm") {
            oss << month_short.at(tm.tm_mon + 1); i += 2; continue;
        } else if (format[i] == 'm' && i + 1 < format.size() && format[i + 1] == 'm') {
            oss << std::setw(2) << std::setfill('0') << (tm.tm_mon + 1); i+=1; continue;
        } else if (format[i] == 'y' && i + 3 < format.size() && format.substr(i, 4) == "yyyy") {
            oss << (tm.tm_year + 1900); i += 3; continue;
        } else if (format[i] == 'y' && i + 1 < format.size() && format[i + 1] == 'y') {
            oss << std::setw(2) << std::setfill('0') << ((tm.tm_year + 1900) % 100); i++; continue;
        } else if (withoutTime) {
            if (format[i] == 'H' && format[i + 1] == 'H') break;
        } 
        if (format[i] == 'H' && i + 1 < format.size() && format[i + 1] == 'H') {
            oss << std::setw(2) << std::setfill('0') << tm.tm_hour; i++;
        } else if (format[i] == 'M' && i + 1 < format.size() && format[i + 1] == 'M') {
            oss << std::setw(2) << std::setfill('0') << tm.tm_min; i++;
        } else if (format[i] == 'S' && i + 1 < format.size() && format[i + 1] == 'S') {
            oss << std::setw(2) << std::setfill('0') << tm.tm_sec; i++;
        } else if (format[i] == 'm' && format[i + 1] == 's') {
            oss << std::setw(3) << std::setfill('0') << millis.count(); i++;
        } else {
            oss << format[i];
        }
    }
    return oss.str();
}

std::string DateTime::toSQL() const{
    return "'" + toString(false, "yyyy-mm-dd HH:MM:SS.ms") + "'";
}

DateTime::DateTime(TimePoint tp) : time_point_(tp) {}

std::tm DateTime::getTm(const TimePoint& tp) const {
    std::time_t tt = Clock::to_time_t(tp);
    std::tm tm{};
    gmtime_r(&tt, &tm); // Использует UTC
    return tm;
}

std::tm DateTime::getTm() const {
    return getTm(time_point_);
}

std::ostream& operator<<(std::ostream& os, DateTime& dt) {
    return os << dt.toString();
}

