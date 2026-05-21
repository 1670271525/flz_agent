#pragma once
#ifndef __FLZ_AGENT_CONFIG_MYSQL_CONFIG_H__
#define __FLZ_AGENT_CONFIG_MYSQL_CONFIG_H__

/**
 * @file    mysql_config.h
 * @brief   MySQL 配置访问封装
 * @date    2026-05
 * @note    RAII: 仅访问 ConfigVar，不持有连接资源
 */

#include <map>
#include <string>

namespace agent {

/**
 * @class MysqlConfig
 * @brief 提供数据库配置读取方法
 */
class MysqlConfig {
public:
    static std::map<std::string, std::map<std::string, std::string> > getDbs();
    static std::string defaultDb();
};

} // namespace agent

#endif // __FLZ_AGENT_CONFIG_MYSQL_CONFIG_H__
