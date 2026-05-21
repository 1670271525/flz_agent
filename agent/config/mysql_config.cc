#include "agent/config/mysql_config.h"

#include "include/config.h"

namespace agent {

static flz::ConfigVar<std::map<std::string, std::map<std::string, std::string> > >::ptr g_mysql_dbs =
    flz::Config::Lookup("mysql.dbs", std::map<std::string, std::map<std::string, std::string> >(), "mysql dbs");

std::map<std::string, std::map<std::string, std::string> > MysqlConfig::getDbs() {
    return g_mysql_dbs->getValue();
}

std::string MysqlConfig::defaultDb() {
    std::map<std::string, std::map<std::string, std::string> > dbs = g_mysql_dbs->getValue();
    if (dbs.empty()) {
        return "agent_main";
    }
    return dbs.begin()->first;
}

} // namespace agent
