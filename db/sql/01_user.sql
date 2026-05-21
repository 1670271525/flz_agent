CREATE TABLE IF NOT EXISTS `user` (
  `user_id`         BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `user_name`       VARCHAR(64)  NOT NULL,
  `user_email`      VARCHAR(128) DEFAULT NULL,
  `user_phone`      VARCHAR(32)  DEFAULT NULL,
  `user_password`   VARCHAR(255) NOT NULL COMMENT 'bcrypt/argon2 哈希',
  `user_status`     TINYINT      NOT NULL DEFAULT 1 COMMENT '0=禁用 1=正常',
  `user_created_at` DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `user_updated_at` DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP
                                            ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`user_id`),
  UNIQUE KEY `uniq_user_name`  (`user_name`),
  UNIQUE KEY `uniq_user_email` (`user_email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
