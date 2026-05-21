CREATE TABLE IF NOT EXISTS `agent_session` (
  `session_id`     VARCHAR(64)  NOT NULL,
  `user_id`        BIGINT UNSIGNED NOT NULL,
  `agent_id`       VARCHAR(64)  NOT NULL,
  `agent_type`     VARCHAR(32)  NOT NULL,
  `last_active_at` DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`session_id`),
  KEY `idx_user_active` (`user_id`, `last_active_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
