/* 
 * COPYRIGHT 2019 FUJITSU LIMITED
 *
 * @file   fams_db.sql
 * @brief  富士通養殖システム管理DB SQLファイル
 * @author anhnguyen
 * @date   3/3/2019
 */

/* 
 * ユーザ情報を格納するテーブル
 */
DROP TABLE IF EXISTS user_informations CASCADE;
CREATE TABLE user_informations(
    id BIGSERIAL,
    username VARCHAR(100) NOT NULL,
    --暗号化して格納する　
    password VARCHAR(140) NOT NULL,
    mail VARCHAR(100) NOT NULL,
    full_name VARCHAR(100) NOT NULL,
    UNIQUE(username),
    PRIMARY KEY(id)
);

/* 
 * システムのバージョン情報を格納するテーブル
 */
DROP TABLE IF EXISTS version_informations CASCADE;
CREATE TABLE version_informations(
    id BIGSERIAL,
    --例: 1.1, 2.1　
    version VARCHAR(5) NOT NULL,
    UNIQUE(version),
    PRIMARY KEY(id)
);

/* 
 * 養殖水槽情報を格納するテーブル
 */
DROP TABLE IF EXISTS cistern_informations CASCADE;
CREATE TABLE cistern_informations(
    id BIGSERIAL,
    name VARCHAR(100) NOT NULL,
    code VARCHAR(100) NOT NULL,
    UNIQUE(code),
    PRIMARY KEY(id)
);

/* 
 * 養殖水槽のカメラ情報を格納するテーブル
 */
DROP TABLE IF EXISTS cistern_camera_details CASCADE;
CREATE TABLE cistern_camera_details(
    id BIGSERIAL,
    cistern_information_id BIGINT NOT NULL,
    ip_address VARCHAR(100) NOT NULL,
    UNIQUE(ip_address),
    PRIMARY KEY(id)
);

/* 
 * ロット情報を格納するテーブル
 */
DROP TABLE IF EXISTS lot_informations CASCADE;
CREATE TABLE lot_informations(
    id BIGSERIAL,
    name VARCHAR(100) NOT NULL,
    description VARCHAR(256) NOT NULL,
    tag VARCHAR(100) NOT NULL,
    unit VARCHAR(100) NOT NULL DEFAULT '個',
    cistern_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 設備情報を格納するテーブル
 */
DROP TABLE IF EXISTS equipment_informations CASCADE;
CREATE TABLE equipment_informations(
    id BIGSERIAL,
    name VARCHAR(100) NOT NULL,
    image_path VARCHAR(256) NOT NULL,
    cistern_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 設備詳細情報を格納するテーブル
 */
DROP TABLE IF EXISTS equipment_details CASCADE;
CREATE TABLE equipment_details(
    id BIGSERIAL,
    name VARCHAR(100) NOT NULL,
    value VARCHAR(100) NOT NULL,
    equipment_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * エラー情報を格納するテーブル
 */
DROP TABLE IF EXISTS error_messages CASCADE;
CREATE TABLE error_messages(
    id BIGSERIAL,
    error_message VARCHAR(256) NOT NULL,
    happened_at TIMESTAMPTZ NOT NULL,
    resource_id BIGINT NOT NULL,
    --　1: ロット | 2: Equipment | 3: Monitor
    resource_kind SMALLINT NOT NULL,
    --　1: 発生してる | 2: 修理した
    status SMALLINT NOT NULL,
    --　Monitor用のみ： | 1: 水温（水槽流入水） | 2: 水温（水槽流出水） | 3: 濁度 | 4: 塩分 | 5: pH
    error_at SMALLINT,
    PRIMARY KEY(id)
);

/* 
 * 生体情報を格納するテーブル
 */
DROP TABLE IF EXISTS organism_informations CASCADE;
CREATE TABLE organism_informations(
    id BIGSERIAL,
    name VARCHAR(100) NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 生体詳細情報を格納するテーブル
 */
DROP TABLE IF EXISTS organism_details CASCADE;
CREATE TABLE organism_details(
    id BIGSERIAL,
    organism_information_id BIGINT NOT NULL,
    type SMALLINT NOT NULL,
    min_value FLOAT,
    min_unit VARCHAR(100),
    max_value FLOAT,
    max_unit VARCHAR(100),
    PRIMARY KEY(id)
);

/* 
 * 養殖水槽に生体が格納される情報を格納するテーブル
 */
DROP TABLE IF EXISTS lot_organism_details CASCADE;
CREATE TABLE lot_organism_details(
    id BIGSERIAL,
    lot_information_id BIGINT NOT NULL,
    organism_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 作業情報を格納するテーブル
 */
DROP TABLE IF EXISTS task_informations CASCADE;
CREATE TABLE task_informations(
    id BIGSERIAL,
    --1: 池入れ, 2: 給餌, 3: 斃死回収, 4: 廃棄, 5: 出荷, 6: 注水・排水, 7: 水質メンテ, 8: 水質モニター　
    type SMALLINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 池入れ情報を格納するテーブル
 */
DROP TABLE IF EXISTS task_pond_inputs CASCADE;
CREATE TABLE task_pond_inputs(
    id BIGSERIAL,
    process_time TIMESTAMPTZ NOT NULL,
    process_user_id BIGINT NOT NULL,
    lot_information_id BIGINT NOT NULL,
    cistern_information_id BIGINT NOT NULL,
    organism_information_id BIGINT NOT NULL,
    organism_number BIGINT NOT NULL,
    organism_tag VARCHAR(256) NOT NULL,
    organism_unit VARCHAR(30) NOT NULL,
    organism_initial_size BIGINT NOT NULL,
    shippable_time TIMESTAMPTZ,
    memo VARCHAR(256),
    task_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 作業給餌入力情報を格納するテーブル
 */
DROP TABLE IF EXISTS task_feeding_inputs CASCADE;
CREATE TABLE task_feeding_inputs(
    id BIGSERIAL,
    process_time TIMESTAMPTZ NOT NULL,
    process_user_id BIGINT NOT NULL,
    feeding_name VARCHAR(100) NOT NULL,
    memo VARCHAR(256),
    task_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 斃死回収入力情報を格納するテーブル
 */
DROP TABLE IF EXISTS task_recovery_inputs CASCADE;
CREATE TABLE task_recovery_inputs(
    id BIGSERIAL,
    process_time TIMESTAMPTZ NOT NULL,
    process_user_id BIGINT NOT NULL,
    memo VARCHAR(256),
    task_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 廃棄入力情報を格納するテーブル
 */
DROP TABLE IF EXISTS task_disposal_inputs CASCADE;
CREATE TABLE task_disposal_inputs(
    id BIGSERIAL,
    process_time TIMESTAMPTZ NOT NULL,
    process_user_id BIGINT NOT NULL,
    disposalType SMALLINT,
    memo VARCHAR(256),
    task_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 出荷入力情報を格納するテーブル
 */
DROP TABLE IF EXISTS task_shipping_inputs CASCADE;
CREATE TABLE task_shipping_inputs(
    id BIGSERIAL,
    process_time TIMESTAMPTZ NOT NULL,
    process_user_id BIGINT NOT NULL,
    shipping_destination VARCHAR(100) NOT NULL,
    memo VARCHAR(256),
    task_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 作業入力詳細情報を格納するテーブル
 */
DROP TABLE IF EXISTS task_input_details CASCADE;
CREATE TABLE task_input_details(
    id BIGSERIAL,
    --　1: 池入れ, 2: 給餌, 3: 斃死回収, 4: 廃棄, 5: 出荷, 6: 注水・排水, 7: 水質メンテ, 8: 水質モニター
    resource_kind SMALLINT NOT NULL,
    resource_id BIGINT NOT NULL,
    lot_information_id BIGINT NOT NULL,
    amount FLOAT,
    PRIMARY KEY(id)
);

/* 
 * 注水・排水情報を格納するテーブル
 */
DROP TABLE IF EXISTS task_water_inputs CASCADE;
CREATE TABLE task_water_inputs(
    id BIGSERIAL,
    process_time TIMESTAMPTZ NOT NULL,
    process_user_id BIGINT NOT NULL,
    cistern_information_id BIGINT NOT NULL,
    --　1:注水 | 2: 排水
    operation SMALLINT NOT NULL,
    amount FLOAT,
    memo VARCHAR(256),
    task_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 水質メンテ情報を格納するテーブル
 */
DROP TABLE IF EXISTS task_maintenance_inputs CASCADE;
CREATE TABLE task_maintenance_inputs(
    id BIGSERIAL,
    process_time TIMESTAMPTZ NOT NULL,
    process_user_id BIGINT NOT NULL,
    cistern_information_id BIGINT NOT NULL,
    --　1: 掃除
    maintenance_type SMALLINT NOT NULL,
    memo VARCHAR(256),
    task_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 水質モニター情報を格納するテーブル
 */
DROP TABLE IF EXISTS task_monitor_inputs CASCADE;
CREATE TABLE task_monitor_inputs(
    id BIGSERIAL,
    process_time TIMESTAMPTZ NOT NULL,
    process_user_id BIGINT NOT NULL,
    cistern_information_id BIGINT NOT NULL,
    monitor_type SMALLINT NOT NULL,
    monitor_value FLOAT,
    memo VARCHAR(256),
    task_information_id BIGINT NOT NULL,
    PRIMARY KEY(id)
);

/* 
 * 観察者情報を格納するテーブル
 */
DROP TABLE IF EXISTS observer_informations CASCADE;
CREATE TABLE observer_informations(
    id BIGSERIAL,
    mail VARCHAR(100) NOT NULL,
    UNIQUE(mail),
    PRIMARY KEY(id)
);

/* 
 * 換水用タンクにてのセンサ情報を格納するテーブル
 */
DROP TABLE IF EXISTS sensor_replace_tank_data CASCADE;
CREATE TABLE sensor_replace_tank_data(
    id BIGSERIAL,
    accumulated_time TIMESTAMPTZ NOT NULL,
    water_level FLOAT NOT NULL,
    water_temp FLOAT NOT NULL,
    UNIQUE(accumulated_time),
    PRIMARY KEY(id)
);

/* 
 * 飼育水槽にてのセンサ情報を格納するテーブル
 */
DROP TABLE IF EXISTS sensor_cistern_data CASCADE;
CREATE TABLE sensor_cistern_data(
    id BIGSERIAL,
    accumulated_time TIMESTAMPTZ NOT NULL,
    cistern_information_id BIGINT NOT NULL,
    inflow_temp FLOAT NOT NULL,
    outflow_temp FLOAT NOT NULL,
    upper_central_ill BIGINT NOT NULL,
    lower_central_ill BIGINT NOT NULL,
    salt FLOAT NOT NULL,
    pH FLOAT NOT NULL,
    UNIQUE(accumulated_time),
    PRIMARY KEY(id)
);

/* 
 * 硝化槽にてのセンサ情報を格納するテーブル
 */
DROP TABLE IF EXISTS sensor_nitrification_tank_data CASCADE;
CREATE TABLE sensor_nitrification_tank_data(
    id BIGSERIAL,
    accumulated_time TIMESTAMPTZ NOT NULL,
    water_temp FLOAT NOT NULL,
    UNIQUE(accumulated_time),
    PRIMARY KEY(id)
);

/* 
 * 外にてのセンサ情報を格納するテーブル
 */
DROP TABLE IF EXISTS sensor_outside_data CASCADE;
CREATE TABLE sensor_outside_data(
    id BIGSERIAL,
    accumulated_time TIMESTAMPTZ NOT NULL,
    room_temp FLOAT NOT NULL,
    humidity FLOAT NOT NULL,
    atmospheric_pressure FLOAT NOT NULL,
    UNIQUE(accumulated_time),
    PRIMARY KEY(id)
);

/* 
 * ゲートウェイ以外のセンサ情報のセンサ情報を格納するテーブル
 */
DROP TABLE IF EXISTS sensor_additional_data CASCADE;
CREATE TABLE sensor_additional_data(
    id BIGSERIAL,
    accumulated_time TIMESTAMPTZ NOT NULL,
    ammonium_ion FLOAT,
    nitrite_ion FLOAT,
    nitrate_ion FLOAT,
    outdoor_temp FLOAT,
    outdoor_humidity FLOAT,
    outdoor_atmospheric_pressure FLOAT,
    outdoor_wind_direction FLOAT,
    outdoor_wind_speed FLOAT,
    sea_water_flow_rate FLOAT,
    air_flow_rate FLOAT,
    wattmeter FLOAT,
    UNIQUE(accumulated_time),
    PRIMARY KEY(id)
);

ALTER TABLE cistern_camera_details ADD FOREIGN KEY(cistern_information_id) REFERENCES cistern_informations(id) ON DELETE CASCADE;
ALTER TABLE lot_informations ADD FOREIGN KEY(cistern_information_id) REFERENCES cistern_informations(id) ON DELETE CASCADE;
ALTER TABLE equipment_informations ADD FOREIGN KEY(cistern_information_id) REFERENCES cistern_informations(id) ON DELETE CASCADE;
ALTER TABLE equipment_details ADD FOREIGN KEY(equipment_information_id) REFERENCES equipment_informations(id) ON DELETE CASCADE;
ALTER TABLE lot_organism_details ADD FOREIGN KEY(lot_information_id) REFERENCES lot_informations(id) ON DELETE CASCADE;
ALTER TABLE lot_organism_details ADD FOREIGN KEY(organism_information_id) REFERENCES organism_informations(id) ON DELETE CASCADE;
ALTER TABLE task_pond_inputs ADD FOREIGN KEY(process_user_id) REFERENCES user_informations(id) ON DELETE CASCADE;
ALTER TABLE task_pond_inputs ADD FOREIGN KEY(lot_information_id) REFERENCES lot_informations(id) ON DELETE CASCADE;
ALTER TABLE task_pond_inputs ADD FOREIGN KEY(cistern_information_id) REFERENCES cistern_informations(id) ON DELETE CASCADE;
ALTER TABLE task_pond_inputs ADD FOREIGN KEY(organism_information_id) REFERENCES organism_informations(id) ON DELETE CASCADE;
ALTER TABLE task_pond_inputs ADD FOREIGN KEY(task_information_id) REFERENCES task_informations(id) ON DELETE CASCADE;
ALTER TABLE task_feeding_inputs ADD FOREIGN KEY(process_user_id) REFERENCES user_informations(id) ON DELETE CASCADE;
ALTER TABLE task_feeding_inputs ADD FOREIGN KEY(task_information_id) REFERENCES task_informations(id) ON DELETE CASCADE;
ALTER TABLE task_recovery_inputs ADD FOREIGN KEY(process_user_id) REFERENCES user_informations(id) ON DELETE CASCADE;
ALTER TABLE task_recovery_inputs ADD FOREIGN KEY(task_information_id) REFERENCES task_informations(id) ON DELETE CASCADE;
ALTER TABLE task_disposal_inputs ADD FOREIGN KEY(process_user_id) REFERENCES user_informations(id) ON DELETE CASCADE;
ALTER TABLE task_disposal_inputs ADD FOREIGN KEY(task_information_id) REFERENCES task_informations(id) ON DELETE CASCADE;
ALTER TABLE task_shipping_inputs ADD FOREIGN KEY(process_user_id) REFERENCES user_informations(id) ON DELETE CASCADE;
ALTER TABLE task_shipping_inputs ADD FOREIGN KEY(task_information_id) REFERENCES task_informations(id) ON DELETE CASCADE;
ALTER TABLE task_input_details ADD FOREIGN KEY(lot_information_id) REFERENCES lot_informations(id) ON DELETE CASCADE;
ALTER TABLE task_water_inputs ADD FOREIGN KEY(process_user_id) REFERENCES user_informations(id) ON DELETE CASCADE;
ALTER TABLE task_water_inputs ADD FOREIGN KEY(cistern_information_id) REFERENCES cistern_informations(id) ON DELETE CASCADE;
ALTER TABLE task_water_inputs ADD FOREIGN KEY(task_information_id) REFERENCES task_informations(id) ON DELETE CASCADE;
ALTER TABLE task_maintenance_inputs ADD FOREIGN KEY(process_user_id) REFERENCES user_informations(id) ON DELETE CASCADE;
ALTER TABLE task_maintenance_inputs ADD FOREIGN KEY(cistern_information_id) REFERENCES cistern_informations(id) ON DELETE CASCADE;
ALTER TABLE task_maintenance_inputs ADD FOREIGN KEY(task_information_id) REFERENCES task_informations(id) ON DELETE CASCADE;
ALTER TABLE task_monitor_inputs ADD FOREIGN KEY(process_user_id) REFERENCES user_informations(id) ON DELETE CASCADE;
ALTER TABLE task_monitor_inputs ADD FOREIGN KEY(cistern_information_id) REFERENCES cistern_informations(id) ON DELETE CASCADE;
ALTER TABLE task_monitor_inputs ADD FOREIGN KEY(task_information_id) REFERENCES task_informations(id) ON DELETE CASCADE;
ALTER TABLE sensor_cistern_data ADD FOREIGN KEY(cistern_information_id) REFERENCES cistern_informations(id) ON DELETE CASCADE;
CREATE INDEX user_informations_A ON user_informations(username);
CREATE INDEX version_informations_A ON version_informations(version);
CREATE INDEX cistern_informations_A ON cistern_informations(name);
CREATE INDEX cistern_camera_details_A ON cistern_camera_details(cistern_information_id);
CREATE INDEX lot_informations_A ON lot_informations(name);
CREATE INDEX lot_informations_C ON lot_informations(cistern_information_id);
CREATE INDEX equipment_informations_A ON equipment_informations(name);
CREATE INDEX equipment_informations_B ON equipment_informations(cistern_information_id);
CREATE INDEX equipment_details_A ON equipment_details(equipment_information_id);
CREATE INDEX error_messages_A ON error_messages(happened_at);
CREATE INDEX error_messages_B ON error_messages(resource_id);
CREATE INDEX error_messages_C ON error_messages(resource_kind);
CREATE INDEX error_messages_D ON error_messages(status);
CREATE INDEX error_messages_E ON error_messages(error_at);
CREATE INDEX organism_informations_A ON organism_informations(name);
CREATE INDEX organism_details_A ON organism_details(organism_information_id);
CREATE INDEX organism_details_B ON organism_details(type);
CREATE INDEX lot_organism_details_A ON lot_organism_details(lot_information_id);
CREATE INDEX lot_organism_details_B ON lot_organism_details(organism_information_id);
CREATE INDEX task_pond_inputs_A ON task_pond_inputs(process_time);
CREATE INDEX task_pond_inputs_B ON task_pond_inputs(lot_information_id);
CREATE INDEX task_pond_inputs_C ON task_pond_inputs(cistern_information_id);
CREATE INDEX task_pond_inputs_D ON task_pond_inputs(organism_information_id);
CREATE INDEX task_pond_inputs_E ON task_pond_inputs(task_information_id);
CREATE INDEX task_feeding_inputs_A ON task_feeding_inputs(process_user_id);
CREATE INDEX task_feeding_inputs_B ON task_feeding_inputs(task_information_id);
CREATE INDEX task_recovery_inputs_A ON task_recovery_inputs(process_user_id);
CREATE INDEX task_recovery_inputs_B ON task_recovery_inputs(task_information_id);
CREATE INDEX task_disposal_inputs_A ON task_disposal_inputs(process_user_id);
CREATE INDEX task_disposal_inputs_B ON task_disposal_inputs(task_information_id);
CREATE INDEX task_shipping_inputs_A ON task_shipping_inputs(process_user_id);
CREATE INDEX task_shipping_inputs_B ON task_shipping_inputs(task_information_id);
CREATE INDEX task_input_details_A ON task_input_details(lot_information_id);
CREATE INDEX task_water_inputs_A ON task_water_inputs(process_user_id);
CREATE INDEX task_water_inputs_B ON task_water_inputs(cistern_information_id);
CREATE INDEX task_water_inputs_C ON task_water_inputs(task_information_id);
CREATE INDEX task_maintenance_inputs_A ON task_maintenance_inputs(process_user_id);
CREATE INDEX task_maintenance_inputs_B ON task_maintenance_inputs(cistern_information_id);
CREATE INDEX task_maintenance_inputs_C ON task_maintenance_inputs(task_information_id);
CREATE INDEX task_monitor_inputs_A ON task_monitor_inputs(process_user_id);
CREATE INDEX task_monitor_inputs_B ON task_monitor_inputs(cistern_information_id);
CREATE INDEX task_monitor_inputs_C ON task_monitor_inputs(task_information_id);
CREATE INDEX observer_informations_A ON observer_informations(mail);
CREATE INDEX sensor_replace_tank_data_A ON sensor_replace_tank_data(accumulated_time);
CREATE INDEX sensor_cistern_data_A ON sensor_cistern_data(accumulated_time);
CREATE INDEX sensor_cistern_data_B ON sensor_cistern_data(cistern_information_id);
CREATE INDEX sensor_nitrification_tank_data_A ON sensor_nitrification_tank_data(accumulated_time);
CREATE INDEX sensor_outside_data_A ON sensor_outside_data(accumulated_time);
CREATE INDEX sensor_additional_data_A ON sensor_additional_data(accumulated_time);
COMMENT ON TABLE user_informations IS 'ユーザ情報を格納するテーブル';
COMMENT ON COLUMN user_informations.password IS '暗号化して格納する　';
COMMENT ON TABLE version_informations IS 'システムのバージョン情報を格納するテーブル';
COMMENT ON COLUMN version_informations.version IS '例: 1.1, 2.1　';
COMMENT ON TABLE cistern_informations IS '養殖水槽情報を格納するテーブル';
COMMENT ON TABLE cistern_camera_details IS '養殖水槽のカメラ情報を格納するテーブル';
COMMENT ON TABLE lot_informations IS 'ロット情報を格納するテーブル';
COMMENT ON TABLE equipment_informations IS '設備情報を格納するテーブル';
COMMENT ON TABLE equipment_details IS '設備詳細情報を格納するテーブル';
COMMENT ON TABLE error_messages IS 'エラー情報を格納するテーブル';
COMMENT ON COLUMN error_messages.resource_kind IS '　1: ロット | 2: Equipment | 3: Monitor';
COMMENT ON COLUMN error_messages.status IS '　1: 発生してる | 2: 修理した';
COMMENT ON COLUMN error_messages.error_at IS '　Monitor用のみ： | 1: 水温（水槽流入水） | 2: 水温（水槽流出水） | 3: 濁度 | 4: 塩分 | 5: pH';
COMMENT ON TABLE organism_informations IS '生体情報を格納するテーブル';
COMMENT ON TABLE organism_details IS '生体詳細情報を格納するテーブル';
COMMENT ON TABLE lot_organism_details IS '養殖水槽に生体が格納される情報を格納するテーブル';
COMMENT ON TABLE task_informations IS '作業情報を格納するテーブル';
COMMENT ON COLUMN task_informations.type IS '1: 池入れ, 2: 給餌, 3: 斃死回収, 4: 廃棄, 5: 出荷, 6: 注水・排水, 7: 水質メンテ, 8: 水質モニター　';
COMMENT ON TABLE task_pond_inputs IS '池入れ情報を格納するテーブル';
COMMENT ON TABLE task_feeding_inputs IS '作業給餌入力情報を格納するテーブル';
COMMENT ON TABLE task_recovery_inputs IS '斃死回収入力情報を格納するテーブル';
COMMENT ON TABLE task_disposal_inputs IS '廃棄入力情報を格納するテーブル';
COMMENT ON TABLE task_shipping_inputs IS '出荷入力情報を格納するテーブル';
COMMENT ON TABLE task_input_details IS '作業入力詳細情報を格納するテーブル';
COMMENT ON COLUMN task_input_details.resource_kind IS '　1: 池入れ, 2: 給餌, 3: 斃死回収, 4: 廃棄, 5: 出荷, 6: 注水・排水, 7: 水質メンテ, 8: 水質モニター';
COMMENT ON TABLE task_water_inputs IS '注水・排水情報を格納するテーブル';
COMMENT ON COLUMN task_water_inputs.operation IS '　1:注水 | 2: 排水';
COMMENT ON TABLE task_maintenance_inputs IS '水質メンテ情報を格納するテーブル';
COMMENT ON COLUMN task_maintenance_inputs.maintenance_type IS '　1: 掃除';
COMMENT ON TABLE task_monitor_inputs IS '水質モニター情報を格納するテーブル';
COMMENT ON TABLE observer_informations IS '観察者情報を格納するテーブル';
COMMENT ON TABLE sensor_replace_tank_data IS '換水用タンクにてのセンサ情報を格納するテーブル';
COMMENT ON TABLE sensor_cistern_data IS '飼育水槽にてのセンサ情報を格納するテーブル';
COMMENT ON TABLE sensor_nitrification_tank_data IS '硝化槽にてのセンサ情報を格納するテーブル';
COMMENT ON TABLE sensor_outside_data IS '外にてのセンサ情報を格納するテーブル';
COMMENT ON TABLE sensor_additional_data IS 'ゲートウェイ以外のセンサ情報のセンサ情報を格納するテーブル';
