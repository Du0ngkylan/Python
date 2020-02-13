/* 
 * COPYRIGHT 2019 FUJITSU LIMITED
 *
 * @file   fams_db.sql
 * @brief  富士通養殖システム管理DB SQLファイル
 * @author ThangLQ
 * @date   20/01/2020
 */

 /* 
 * 飼育水槽にてのセンサ情報を格納するテーブル
 */

-- userInformation table
DROP TABLE IF EXISTS userInformation CASCADE;
CREATE TABLE userInformation(
    tenantCode VARCHAR(5) NOT NULL,
    userId BIGSERIAL,
    account VARCHAR(64) NOT NULL,
    password VARCHAR(64) NOT NULL,
    name VARCHAR(64) NOT NULL,
    role SMALLINT NOT NULL,
    mail VARCHAR(100) NOT NULL,
    UNIQUE(account),
    PRIMARY KEY(userId)
);

-- activitiyInformation table
DROP TABLE IF EXISTS activitiyInformation CASCADE;
CREATE TABLE activitiyInformation(
    tenantCode VARCHAR(5) NOT NULL,
    activitiyId BIGSERIAL,
    type SMALLINT,
    date TIMESTAMPTZ NOT NULL,
    description VARCHAR(256) NOT NULL,
    userId BIGINT NOT NULL,
    UNIQUE(userId),
    PRIMARY KEY(activitiyId)
);

-- cameraInformation table
DROP TABLE IF EXISTS cameraInformation CASCADE;
CREATE TABLE cameraInformation(
    tenantCode VARCHAR(5) NOT NULL,
    cameraId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    definedName VARCHAR(64) NOT NULL,
    host VARCHAR(64) NOT NULL,
    port VARCHAR(20) NOT NULL,
    PRIMARY KEY(cameraId)
);

-- announcementInformation table
DROP TABLE IF EXISTS announcementInformation;
CREATE TABLE announcementInformation(
    tenantCode VARCHAR(5) NOT NULL,
    announcementId BIGSERIAL,
    title VARCHAR(64) NOT NULL,
    content VARCHAR(64) NOT NULL,
    fromDate TIMESTAMPTZ NOT NULL,
    toDate TIMESTAMPTZ NOT NULL,
    createDate TIMESTAMPTZ NOT NULL,
    updateDate TIMESTAMPTZ NOT NULL,
    PRIMARY KEY(announcementId)
);

-- errorMessageInformation table
DROP TABLE IF EXISTS errorMessageInformation CASCADE;
CREATE TABLE errorMessageInformation(
    tenantCode VARCHAR(5) NOT NULL,
    errorMessageId BIGSERIAL,
    happenedDate TIMESTAMPTZ NOT NULL,
    resourceType SMALLINT,
    resourceId BIGINT,
    status SMALLINT,
    PRIMARY KEY(errorMessageId)
);

-- cyclicThreadConfig table
DROP TABLE IF EXISTS cyclicThreadConfig CASCADE;
CREATE TABLE cyclicThreadConfig(
    tenantCode VARCHAR(5) NOT NULL,
    cyclicThreadConfigId BIGSERIAL,
    periodMonitoringTimer BIGINT NOT NULL,
    monitoringCounter BIGINT NOT NULL,
    sensorId BIGINT NOT NULL,
    PRIMARY KEY(cyclicThreadConfigId)
);

-- sensorValueInformation table
DROP TABLE IF EXISTS sensorValueInformation CASCADE;
CREATE TABLE sensorValueInformation(
    tenantCode VARCHAR(5) NOT NULL,
    sensorValueId BIGSERIAL,
    value NUMERIC NOT NULL,
    sensorId BIGINT NOT NULL,
    PRIMARY KEY(sensorValueId)
);

-- sensorInformation table
DROP TABLE IF EXISTS sensorInformation CASCADE;
CREATE TABLE sensorInformation(
    tenantCode VARCHAR(5) NOT NULL,
    sensorId BIGSERIAL,
    date TIMESTAMPTZ NOT NULL,
    name VARCHAR(64) NOT NULL,
    definedName VARCHAR(64) NOT NULL,
    type SMALLINT NOT NULL,
    rtuId VARCHAR(20) NOT NULL,
    rtuChannel VARCHAR(20) NOT NULL,
    unit VARCHAR(16) NOT NULL,
    PRIMARY KEY(sensorId)
);

-- areaSensorInformation table
DROP TABLE IF EXISTS areaSensorInformation CASCADE;
CREATE TABLE areaSensorInformation(
    tenantCode VARCHAR(5) NOT NULL,
    areaId BIGINT NOT NULL,
    sensorId BIGINT NOT NULL
);

-- areaInformation table
DROP TABLE IF EXISTS areaInformation CASCADE;
CREATE TABLE areaInformation(
    tenantCode VARCHAR(5) NOT NULL,
    areaId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    PRIMARY KEY(areaId)
);
-- cisternSensorInformation table
DROP TABLE IF EXISTS cisternSensorInformation CASCADE;
CREATE TABLE cisternSensorInformation(
    tenantCode VARCHAR(5) NOT NULL,
    cisternId BIGINT NOT NULL,
    sensorId BIGINT NOT NULL
);

-- cisternInformation table
DROP TABLE IF EXISTS cisternInformation CASCADE;
CREATE TABLE cisternInformation(
    tenantCode VARCHAR(5) NOT NULL,
    cisternId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    masterCisternId BIGINT NOT NULL,
    areaId BIGINT NOT NULL,
    UNIQUE(masterCisternId),
    UNIQUE(areaId),
    PRIMARY KEY(cisternId)
);

-- masterCisternInformation table
DROP TABLE IF EXISTS masterCisternInformation CASCADE;
CREATE TABLE masterCisternInformation(
    tenantCode VARCHAR(5) NOT NULL,
    masterCisternId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    capacity VARCHAR(20) NOT NULL,
    netX VARCHAR(20) NOT NULL,
    netY VARCHAR(20) NOT NULL,
    PRIMARY KEY(masterCisternId)
);

-- lotInformation table
DROP TABLE IF EXISTS lotInformation CASCADE;
CREATE TABLE lotInformation(
    tenantCode VARCHAR(5) NOT NULL,
    lotId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    lotNumber VARCHAR(64) NOT NULL,
    currentAmount BIGINT,
    shippableDate TIMESTAMPTZ NOT NULL,
    cisternId BIGINT NOT NULL,
    organismId BIGINT NOT NULL,
    UNIQUE(cisternId),
    UNIQUE(organismId),
    PRIMARY KEY(lotId)
);

-- periodMonitoringInformation table
DROP TABLE IF EXISTS periodMonitoringInformation CASCADE;
CREATE TABLE periodMonitoringInformation(
    tenantCode VARCHAR(5) NOT NULL,
    periodMonitoringId BIGSERIAL,
    periodMonitoringCounter BIGINT NOT NULL,
    monitoringCounter BIGINT NOT NULL,
    initialPeriodMonitoringTimer BIGINT NOT NULL,
    alertCounter BIGINT NOT NULL,
    sensorId BIGINT NOT NULL,	
    PRIMARY KEY(periodMonitoringId)
);

-- organismInformation table
DROP TABLE IF EXISTS organismInformation CASCADE;
CREATE TABLE organismInformation(
    tenantCode VARCHAR(5) NOT NULL,
    organismId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    PRIMARY KEY(organismId)
);

-- thresholdInformation table
DROP TABLE IF EXISTS thresholdInformation CASCADE;
CREATE TABLE thresholdInformation(
    tenantCode VARCHAR(5) NOT NULL,
    thresholdId BIGSERIAL,
    min NUMERIC NOT NULL,
    max NUMERIC NOT NULL,
    organismId BIGINT NOT NULL,
    masterThresholdId BIGINT NOT NULL,
    UNIQUE(organismId),
    UNIQUE(masterThresholdId),
    PRIMARY KEY(thresholdId)
);

-- masterThresholdInformation table
DROP TABLE IF EXISTS masterThresholdInformation CASCADE;
CREATE TABLE masterThresholdInformation(
    tenantCode VARCHAR(5) NOT NULL,
    masterThresholdId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    unit VARCHAR(16) NOT NULL,
    PRIMARY KEY(masterThresholdId)
);

-- taskInformation table
DROP TABLE IF EXISTS taskInformation CASCADE;
CREATE TABLE taskInformation(
    tenantCode VARCHAR(5) NOT NULL,
    taskId BIGSERIAL,
    type SMALLINT NOT NULL,
    assigneeUserId BIGINT NOT NULL,
    date TIMESTAMPTZ,
    status SMALLINT NOT NULL,
    note VARCHAR(2048) NOT NULL,
    createUserID BIGINT NOT NULL,
    createDate TIMESTAMPTZ NOT NULL,
    updateUserId BIGINT NOT NULL,
    updateDate TIMESTAMPTZ NOT NULL,
    UNIQUE(assigneeUserId),
    UNIQUE(createUserID),
    UNIQUE(updateUserId),
    PRIMARY KEY(taskId)
);

-- taskPondInput table
DROP TABLE IF EXISTS taskPondInput CASCADE;
CREATE TABLE taskPondInput(
    tenantCode VARCHAR(5) NOT NULL,
    taskPondInputId BIGSERIAL,
    indentifyName VARCHAR(64) NOT NULL,
    amount BIGINT NOT NULL,
    initialSize NUMERIC NOT NULL,
    lotId BIGINT NOT NULL,
    taskId BIGINT NOT NULL,
    UNIQUE(lotId),
    UNIQUE(taskId),
    PRIMARY KEY(taskPondInputId)
);

-- taskRelocation table
DROP TABLE IF EXISTS taskRelocation CASCADE;
CREATE TABLE taskRelocation(
	tenantCode VARCHAR(5) NOT NULL,
	taskRelocationId BIGSERIAL,
	amount BIGINT NOT NULL,
	fromIndentifyName VARCHAR(64) NOT NULL,
	fromCisternId BIGINT NOT NULL,
	fromLotId BIGINT NOT NULL,
	toIndentifyName VARCHAR(64) NOT NULL,
	toCisternId BIGINT NOT NULL,
	toLotId BIGINT NOT NULL,
	taskId BIGINT NOT NULL,
	UNIQUE(fromCisternId),
	UNIQUE(fromLotId),
	UNIQUE(toCisternId),
	UNIQUE(toLotId),
	UNIQUE(taskId),
	PRIMARY KEY(taskRelocationId)
);

-- taskStock table
DROP TABLE IF EXISTS taskStock CASCADE;
CREATE TABLE taskStock(
    tenantCode VARCHAR(5) NOT NULL,
    taskStockId BIGSERIAL,
    stockType SMALLINT NOT NULL, 
    stockAmount NUMERIC NOT NULL,
    taskId BIGINT NOT NULL,
    UNIQUE(taskId),
    PRIMARY KEY(taskStockId)
);

-- taskStockSeedInformation table
DROP TABLE IF EXISTS taskStockSeedInformation CASCADE;
CREATE TABLE taskStockSeedInformation(
    tenantCode VARCHAR(5) NOT NULL,
    taskStockId BIGINT NOT NULL,
    stockSeedId SMALLINT NOT NULL,
    UNIQUE(taskStockId),
    UNIQUE(stockSeedId)
);

-- stockSeedInformation table
DROP TABLE IF EXISTS stockSeedInformation CASCADE;
CREATE TABLE stockSeedInformation(
    tenantCode VARCHAR(5) NOT NULL,
    stockSeedId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    currentAmount NUMERIC NOT NULL,
    masterOrganismId BIGINT NOT NULL,
    UNIQUE(masterOrganismId),
    PRIMARY KEY(stockSeedId)
);

-- taskStockFeedInformation table
DROP TABLE IF EXISTS taskStockFeedInformation CASCADE;
CREATE TABLE taskStockFeedInformation(
    tenantCode VARCHAR(5) NOT NULL,
    taskStockId BIGINT NOT NULL,
    stockFeedId SMALLINT NOT NULL,
    UNIQUE(taskStockId),
    UNIQUE(stockFeedId)
);

-- stockFeedInformation table
DROP TABLE IF EXISTS stockFeedInformation CASCADE;
CREATE TABLE stockFeedInformation(
    tenantCode VARCHAR(5) NOT NULL,
    stockFeedId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    currentAmount NUMERIC NOT NULL,
    masterFeedId BIGINT NOT NULL,
    UNIQUE(masterFeedId),
    PRIMARY KEY(stockFeedId)
);

-- masterFeedInformation table
DROP TABLE IF EXISTS masterFeedInformation CASCADE;
CREATE TABLE masterFeedInformation(
    tenantCode VARCHAR(5) NOT NULL,
    masterFeedId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    unitAmount VARCHAR(64) NOT NULL,
    visible BOOLEAN NOT NULL,
    PRIMARY KEY(masterFeedId)
);

-- taskStockNutrientInformation table
DROP TABLE IF EXISTS taskStockNutrientInformation CASCADE;
CREATE TABLE taskStockNutrientInformation(
    tenantCode VARCHAR(5) NOT NULL,
    taskStockId BIGINT NOT NULL,
    stockNutrientId SMALLINT NOT NULL,
    UNIQUE(taskStockId),
    UNIQUE(stockNutrientId)
);

-- stockNutrientInformation table
DROP TABLE IF EXISTS stockNutrientInformation CASCADE;
CREATE TABLE stockNutrientInformation(
    tenantCode VARCHAR(5) NOT NULL,
    stockNutrientId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    currentAmount NUMERIC NOT NULL,
    masterNutrientId BIGINT NOT NULL,
    UNIQUE(masterNutrientId),
    PRIMARY KEY(stockNutrientId)
);

-- masterNutrientInformation table
DROP TABLE IF EXISTS masterNutrientInformation CASCADE;
CREATE TABLE masterNutrientInformation(
    tenantCode VARCHAR(5) NOT NULL,
    masterNutrientId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    unitAmount VARCHAR(64) NOT NULL,
    visible BOOLEAN NOT NULL,
    PRIMARY KEY(masterNutrientId)
);

-- taskStockMedicineInformation table
DROP TABLE IF EXISTS taskStockMedicineInformation CASCADE;
CREATE TABLE taskStockMedicineInformation(
    tenantCode VARCHAR(5) NOT NULL,
    taskStockId BIGINT NOT NULL,
    stockMedicineId SMALLINT NOT NULL,
    UNIQUE(taskStockId),
    UNIQUE(stockMedicineId)
);

-- stockMedicineInformation table
DROP TABLE IF EXISTS stockMedicineInformation CASCADE;
CREATE TABLE stockMedicineInformation(
    tenantCode VARCHAR(5) NOT NULL,
    stockMedicineId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    currentAmount NUMERIC NOT NULL,
    masterMedicineId BIGINT NOT NULL,
    UNIQUE(masterMedicineId),
    PRIMARY KEY(stockMedicineId)
);

-- masterMedicineInformation table
DROP TABLE IF EXISTS masterMedicineInformation CASCADE;
CREATE TABLE masterMedicineInformation(
    tenantCode VARCHAR(5) NOT NULL,
    masterMedicineId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    unitAmount VARCHAR(64) NOT NULL,
    visible BOOLEAN NOT NULL,
    PRIMARY KEY(masterMedicineId)
);

-- taskSupply table
DROP TABLE IF EXISTS taskSupply CASCADE;
CREATE TABLE taskSupply(
    tenantCode VARCHAR(5) NOT NULL,
    taskSupplyId BIGSERIAL,
    supplyType SMALLINT NOT NULL,
    useStockDetailId BIGINT NOT NULL,
    taskId BIGINT NOT NULL,
    UNIQUE(useStockDetailId),
    UNIQUE(taskId),
    PRIMARY KEY(taskSupplyId)
);

-- taskSupplySeedInformation table
DROP TABLE IF EXISTS taskSupplySeedInformation CASCADE;
CREATE TABLE taskSupplySeedInformation(
    tenantCode VARCHAR(5) NOT NULL,
    taskSupplyId BIGINT NOT NULL,
    stockSeedId BIGINT NOT NULL,
    UNIQUE(taskSupplyId),
    UNIQUE(stockSeedId)
);

-- taskSupplyFeedInformation table
DROP TABLE IF EXISTS taskSupplyFeedInformation CASCADE;
CREATE TABLE taskSupplyFeedInformation(
    tenantCode VARCHAR(5) NOT NULL,
    taskSupplyId BIGINT NOT NULL,
    stockFeedId BIGINT NOT NULL,
    UNIQUE(taskSupplyId),
    UNIQUE(stockFeedId)
);

-- taskSupplyNutrientInformation table
DROP TABLE IF EXISTS taskSupplyNutrientInformation CASCADE;
CREATE TABLE taskSupplyNutrientInformation(
    tenantCode VARCHAR(5) NOT NULL,
    taskSupplyId BIGINT NOT NULL,
    stockNutrientId BIGINT NOT NULL,
    UNIQUE(taskSupplyId),
    UNIQUE(stockNutrientId)
);

-- taskSupplyMedicineInformation table
DROP TABLE IF EXISTS taskSupplyMedicineInformation CASCADE;
CREATE TABLE taskSupplyMedicineInformation(
    tenantCode VARCHAR(5) NOT NULL,
    taskSupplyId BIGINT NOT NULL,
    stockMedicineId BIGINT NOT NULL,
    UNIQUE(taskSupplyId),
    UNIQUE(stockMedicineId)
);

-- supplyAmountInformation table
DROP TABLE IF EXISTS supplyAmountInformation CASCADE;
CREATE TABLE supplyAmountInformation(
    tenantCode VARCHAR(5) NOT NULL,
    supplyAmountId BIGINT NOT NULL,
    amount NUMERIC NOT NULL,
    leftover NUMERIC NOT NULL,
    lotId BIGINT NOT NULL,
    taskSupplyId BIGINT NOT NULL,
    UNIQUE(lotId),
    PRIMARY KEY(supplyAmountId)
);

-- taskRecovery table
DROP TABLE IF EXISTS taskRecovery CASCADE;
CREATE TABLE taskRecovery(
    tenantCode VARCHAR(5) NOT NULL,
    taskRecoveryId BIGSERIAL,
    organismAmountId BIGINT NOT NULL,
    taskId BIGINT NOT NULL,
    UNIQUE(taskId),
    PRIMARY KEY(taskRecoveryId)
);

-- taskAdjustment table
DROP TABLE IF EXISTS taskAdjustment CASCADE;
CREATE TABLE taskAdjustment(
    tenantCode VARCHAR(5) NOT NULL,
    taskAdjustmentId BIGSERIAL,
    organismAmountId BIGINT NOT NULL,
    taskId BIGINT NOT NULL,
    UNIQUE(taskId),
    PRIMARY KEY(taskAdjustmentId)
);

-- taskShipping table
DROP TABLE IF EXISTS taskShipping CASCADE;
CREATE TABLE taskShipping(
    tenantCode VARCHAR(5) NOT NULL,
    taskShippingId BIGSERIAL,
    description VARCHAR(64) NOT NULL,
    organismAmountId BIGINT NOT NULL,
    taskId BIGINT NOT NULL,
    UNIQUE(taskId),
    PRIMARY KEY(taskShippingId)
);

-- taskSampling table
DROP TABLE IF EXISTS taskSampling CASCADE;
CREATE TABLE taskSampling(
    tenantCode VARCHAR(5) NOT NULL,
    taskSamplingId BIGSERIAL,
    type SMALLINT NOT NULL,
    organismAmountId BIGINT NOT NULL,
    taskId BIGINT NOT NULL,
    UNIQUE(taskId),
    PRIMARY KEY(taskSamplingId)
);

-- taskMonitoring table
DROP TABLE IF EXISTS taskMonitoring CASCADE;
CREATE TABLE taskMonitoring(
    tenantCode VARCHAR(5) NOT NULL,
    taskMonitoringId BIGSERIAL,
    masterMonitoringId BIGINT NOT NULL,
    taskId BIGINT NOT NULL,
    UNIQUE(taskId),
    UNIQUE(masterMonitoringId),
    PRIMARY KEY(taskMonitoringId)
);

-- taskDayBook table
DROP TABLE IF EXISTS taskDayBook CASCADE;
CREATE TABLE taskDayBook(
    tenantCode VARCHAR(5) NOT NULL,
    taskDayBookId BIGSERIAL,
    title VARCHAR(64) NOT NULL,
    taskId BIGINT NOT NULL,
    UNIQUE(taskId),
    PRIMARY KEY(taskDayBookId)
);

-- organismAmountInformation table
DROP TABLE IF EXISTS organismAmountInformation CASCADE;
CREATE TABLE organismAmountInformation(
    tenantCode VARCHAR(5) NOT NULL,
    organismAmountId BIGSERIAL,
    amount BIGINT NOT NULL,
    lotId BIGINT NOT NULL,
    taskType SMALLINT NOT NULL,
    subTaskId BIGINT NOT NULL,
    UNIQUE(lotId),
    PRIMARY KEY(organismAmountId)
);

-- samplingMeasurementInformation table
DROP TABLE IF EXISTS samplingMeasurementInformation CASCADE;
CREATE TABLE samplingMeasurementInformation(
    tenantCode VARCHAR(5) NOT NULL,
    samplingMeasurementId BIGSERIAL,
    lotId BIGINT NOT NULL,
    masterSamplingId BIGINT NOT NULL,
    value NUMERIC NOT NULL,
    taskSamplingId BIGINT NOT NULL,
    UNIQUE(lotId),
    UNIQUE(masterSamplingId),
    PRIMARY KEY(samplingMeasurementId)
);

-- masterSamplingInformation table
DROP TABLE IF EXISTS masterSamplingInformation CASCADE;
CREATE TABLE masterSamplingInformation(
	tenantCode VARCHAR(5) NOT NULL,
	masterSamplingId BIGSERIAL,
	name VARCHAR(64) NOT NULL,
	unit VARCHAR(16) NOT NULL,
	visible BOOLEAN NOT NULL,
	PRIMARY KEY(masterSamplingId)
);
-- monitoringMeasurementInformation table
DROP TABLE IF EXISTS monitoringMeasurementInformation CASCADE;
CREATE TABLE monitoringMeasurementInformation(
    tenantCode VARCHAR(5) NOT NULL,
    monitoringMeasurementId BIGSERIAL,
    lotId BIGINT NOT NULL,
    value NUMERIC NOT NULL,
    taskMonitoringId BIGINT NOT NULL,
    UNIQUE(lotId),
    PRIMARY KEY(monitoringMeasurementId)
);

-- masterMonitoringInformation table
DROP TABLE IF EXISTS masterMonitoringInformation CASCADE;
CREATE TABLE masterMonitoringInformation(
    tenantCode VARCHAR(5) NOT NULL,
    masterMonitoringId BIGSERIAL,
    name VARCHAR(64) NOT NULL,
    unit VARCHAR(16) NOT NULL,
    visible BOOLEAN NOT NULL,
    PRIMARY KEY(masterMonitoringId)
);

-- userInformation
-- userInformation 1-1 taskInformation(assigneeUserId or createUserID or updateUserId)
ALTER TABLE userInformation ADD FOREIGN KEY(userId) REFERENCES taskInformation(assigneeUserId) ON DELETE CASCADE;
ALTER TABLE userInformation ADD FOREIGN KEY(userId) REFERENCES taskInformation(createUserID) ON DELETE CASCADE;
ALTER TABLE userInformation ADD FOREIGN KEY(userId) REFERENCES taskInformation(updateUserId) ON DELETE CASCADE;
-- userInformation 1-n activitiyInformation
ALTER TABLE activitiyInformation ADD FOREIGN KEY(userId) REFERENCES userInformation(userId) ON DELETE CASCADE;
-- taskInformation 1-1 taskPondInput
ALTER TABLE taskInformation ADD FOREIGN KEY(taskId) REFERENCES taskPondInput(taskId) ON DELETE CASCADE;
-- taskInformation 1-1 taskRelocation
ALTER TABLE taskInformation ADD FOREIGN KEY(taskId) REFERENCES taskRelocation(taskId) ON DELETE CASCADE;
-- taskInformation 1-1 taskStock
ALTER TABLE taskInformation  ADD FOREIGN KEY(taskId) REFERENCES taskStock(taskId) ON DELETE CASCADE;
-- taskInformation 1-1 taskSupply
ALTER TABLE taskInformation ADD FOREIGN KEY(taskId) REFERENCES taskSupply(taskId) ON DELETE CASCADE;
-- taskInformation 1-1 taskRecovery
ALTER TABLE taskInformation ADD FOREIGN KEY(taskId) REFERENCES taskRecovery(taskId) ON DELETE CASCADE;
-- taskInformation 1-1 taskAdjustment
ALTER TABLE taskInformation ADD FOREIGN KEY(taskId) REFERENCES taskAdjustment(taskId) ON DELETE CASCADE;
-- taskInformation 1-1 taskShipping
ALTER TABLE taskInformation ADD FOREIGN KEY(taskId) REFERENCES taskShipping(taskId) ON DELETE CASCADE;
-- taskInformation 1-1 taskSampling
ALTER TABLE taskInformation ADD FOREIGN KEY(taskId) REFERENCES taskSampling(taskId) ON DELETE CASCADE;
-- taskInformation 1-1 taskMonitoring
ALTER TABLE taskInformation ADD FOREIGN KEY(taskId) REFERENCES taskMonitoring(taskId) ON DELETE CASCADE;
-- taskInformation 1-1 taskDayBook
ALTER TABLE taskInformation ADD FOREIGN KEY(taskId) REFERENCES taskDayBook(taskId) ON DELETE CASCADE;

-- taskRelocation
-- taskRelocation(fromCisternId, toCisternId) 1-1 cisternInformation
ALTER TABLE cisternInformation ADD FOREIGN KEY(cisternId) REFERENCES taskRelocation(fromCisternId) ON DELETE CASCADE;
ALTER TABLE cisternInformation ADD FOREIGN KEY(cisternId) REFERENCES taskRelocation(toCisternId) ON DELETE CASCADE;
-- taskRelocation(fromLotId, toLotId) 1-1 lotInformation(lotId)
ALTER TABLE lotInformation ADD FOREIGN KEY(lotId) REFERENCES taskRelocation(fromLotId) ON DELETE CASCADE;
ALTER TABLE lotInformation ADD FOREIGN KEY(lotId) REFERENCES taskRelocation(toLotId) ON DELETE CASCADE;

-- taskStock
-- taskStock 1-1 taskStockSeedInformation
ALTER TABLE taskStock ADD FOREIGN KEY(taskStockId) REFERENCES taskStockSeedInformation(taskStockId) ON DELETE CASCADE;
-- taskStockSeedInformation 1-1 stockSeedInformation
ALTER TABLE stockSeedInformation ADD FOREIGN KEY(stockSeedId) REFERENCES taskStockSeedInformation(stockSeedId) ON DELETE CASCADE;
-- taskStock 1-1 taskStockFeedInformation
ALTER TABLE taskStock ADD FOREIGN KEY(taskStockId) REFERENCES taskStockFeedInformation(taskStockId) ON DELETE CASCADE;
-- taskStockFeedInformation 1-1 stockFeedInformation
ALTER TABLE stockFeedInformation ADD FOREIGN KEY(stockFeedId) REFERENCES taskStockFeedInformation(stockFeedId) ON DELETE CASCADE;
-- stockFeedInformation 1-1 masterFeedInformation
ALTER TABLE masterFeedInformation ADD FOREIGN KEY(masterFeedId) REFERENCES stockFeedInformation(masterFeedId) ON DELETE CASCADE;
-- taskStock 1-1 taskStockNutrientInformation
ALTER TABLE taskStock ADD FOREIGN KEY(taskStockId) REFERENCES taskStockNutrientInformation(taskStockId) ON DELETE CASCADE;
-- taskStockNutrientInformation 1-1 stockNutrientInformation
ALTER TABLE stockNutrientInformation ADD FOREIGN KEY(stockNutrientId) REFERENCES taskStockNutrientInformation(stockNutrientId) ON DELETE CASCADE;
-- stockNutrientInformation 1-1 masterNutrientInformation
ALTER TABLE masterNutrientInformation ADD FOREIGN KEY(masterNutrientId) REFERENCES stockNutrientInformation(masterNutrientId) ON DELETE CASCADE;
-- taskStock 1-1 taskStockMedicineInformation
ALTER TABLE taskStock ADD FOREIGN KEY(taskStockId) REFERENCES taskStockMedicineInformation(taskStockId) ON DELETE CASCADE;
-- taskStockMedicineInformation 1-1 stockMedicineInformation
ALTER TABLE stockMedicineInformation ADD FOREIGN KEY(stockMedicineId) REFERENCES taskStockMedicineInformation(stockMedicineId) ON DELETE CASCADE;
-- stockMedicineInformation 1-1 masterMedicineInformation
ALTER TABLE masterMedicineInformation ADD FOREIGN KEY(masterMedicineId) REFERENCES stockMedicineInformation(masterMedicineId) ON DELETE CASCADE;
-- taskSupply
-- taskSupply 1-1 taskSupplySeedInformation
ALTER TABLE taskSupply ADD FOREIGN KEY(taskSupplyId) REFERENCES taskSupplySeedInformation(taskSupplyId) ON DELETE CASCADE;
-- taskSupplySeedInformation 1-1 stockSeedInformation
ALTER TABLE stockSeedInformation ADD FOREIGN KEY(stockSeedId) REFERENCES taskSupplySeedInformation(stockSeedId) ON DELETE CASCADE;
-- taskSupply 1-1 taskSupplyFeedInformation
ALTER TABLE taskSupply ADD FOREIGN KEY(taskSupplyId) REFERENCES taskSupplyFeedInformation(taskSupplyId) ON DELETE CASCADE;
-- taskSupplyFeedInformation 1-1 stockFeedInformation
ALTER TABLE stockFeedInformation ADD FOREIGN KEY(stockFeedId) REFERENCES taskSupplyFeedInformation(stockFeedId) ON DELETE CASCADE;
-- taskSupply 1-1 taskSupplyNutrientInformation
ALTER TABLE taskSupply ADD FOREIGN KEY(taskSupplyId) REFERENCES taskSupplyNutrientInformation(taskSupplyId) ON DELETE CASCADE;
-- taskSupplyNutrientInformation 1-1 stockNutrientInformation
ALTER TABLE stockNutrientInformation ADD FOREIGN KEY(stockNutrientId) REFERENCES taskSupplyNutrientInformation(stockNutrientId) ON DELETE CASCADE;
-- taskSupply 1-1 taskSupplyMedicineInformation
ALTER TABLE taskSupply ADD FOREIGN KEY(taskSupplyId) REFERENCES taskSupplyMedicineInformation(taskSupplyId) ON DELETE CASCADE;
-- taskSupplyMedicineInformation 1-1 stockMedicineInformation
ALTER TABLE stockMedicineInformation ADD FOREIGN KEY(stockMedicineId) REFERENCES taskSupplyMedicineInformation(stockMedicineId) ON DELETE CASCADE;
-- taskSupply 1-1 supplyAmountInformation
ALTER TABLE supplyAmountInformation ADD FOREIGN KEY(taskSupplyId) REFERENCES taskSupply(taskSupplyId) ON DELETE CASCADE;
-- lotInformation 1-1 supplyAmountInformation
ALTER TABLE lotInformation ADD FOREIGN KEY(lotId) REFERENCES supplyAmountInformation(lotId) ON DELETE CASCADE;

-- taskRecovery 1-n organismAmountInformation
ALTER TABLE organismAmountInformation ADD FOREIGN KEY(subTaskId) REFERENCES taskRecovery(taskRecoveryId) ON DELETE CASCADE;
-- taskAdjustment 1-n organismAmountInformation
ALTER TABLE organismAmountInformation ADD FOREIGN KEY(subTaskId) REFERENCES taskAdjustment(taskAdjustmentId) ON DELETE CASCADE;

-- taskShipping 1-n organismAmountInformation
ALTER TABLE organismAmountInformation ADD FOREIGN KEY(subTaskId) REFERENCES taskShipping(taskShippingId) ON DELETE CASCADE;

-- taskSampling 1-n organismAmountInformation
ALTER TABLE organismAmountInformation ADD FOREIGN KEY(subTaskId) REFERENCES taskSampling(taskSamplingId) ON DELETE CASCADE;

-- taskSampling 1-n samplingMeasurementInformation
ALTER TABLE samplingMeasurementInformation ADD FOREIGN KEY(taskSamplingId) REFERENCES taskSampling(taskSamplingId) ON DELETE CASCADE;
-- samplingMeasurementInformation 1-1 masterSamplingInformation
ALTER TABLE mastersamplinginformation ADD FOREIGN KEY(masterSamplingId) REFERENCES samplingMeasurementInformation(masterSamplingId) ON DELETE CASCADE;

-- taskMonitoring 1-n monitoringMeasurementInformation
ALTER TABLE monitoringMeasurementInformation ADD FOREIGN KEY(taskMonitoringId) REFERENCES taskMonitoring(taskMonitoringId) ON DELETE CASCADE;

-- taskMonitoring 1-1 masterMonitoringInformation
ALTER TABLE taskMonitoring ADD FOREIGN KEY(masterMonitoringId) REFERENCES masterMonitoringInformation(masterMonitoringId) ON DELETE CASCADE;

-- sensorInformation
-- sensorInformation 1-n areaSensorInformation
ALTER TABLE areaSensorInformation ADD FOREIGN KEY(sensorId) REFERENCES sensorInformation(sensorId) ON DELETE CASCADE;
-- sensorInformation 1-n sensorValueInformation
ALTER TABLE sensorValueInformation ADD FOREIGN KEY(sensorId) REFERENCES sensorInformation(sensorId) ON DELETE CASCADE;
-- sensorInformation 1-0 cyclicThreadConfig
-- sensorInformation 1-0 periodMonitoringInformation
-- sensorInformation 1-n cisternSensorInformation
ALTER TABLE cisternSensorInformation ADD FOREIGN KEY(sensorId) REFERENCES sensorInformation(sensorId) ON DELETE CASCADE;

-- areaInformation 1-n areaSensorInformation
ALTER TABLE areaSensorInformation ADD FOREIGN KEY(areaId) REFERENCES areaInformation(areaId) ON DELETE CASCADE;
-- areaInformation 1-n cisternInformation
ALTER TABLE cisternInformation ADD FOREIGN KEY(areaId) REFERENCES areaInformation(areaId) ON DELETE CASCADE;

-- cisternInformation
-- cisternInformation 1-n cisternSensorInformation
ALTER TABLE cisternSensorInformation ADD FOREIGN KEY(cisternId) REFERENCES cisternInformation(cisternId) ON DELETE CASCADE;

-- cisternInformation n-1 masterCisternInformation
ALTER TABLE cisternInformation ADD FOREIGN KEY(masterCisternId) REFERENCES masterCisternInformation(masterCisternId) ON DELETE CASCADE;
-- cisternInformation 1-n lotInformation
ALTER TABLE lotInformation ADD FOREIGN KEY(cisternId) REFERENCES cisternInformation(cisternId) ON DELETE CASCADE;

-- lotInformation
-- lotInformation n-1 organismInformation
ALTER TABLE lotInformation ADD FOREIGN KEY(organismId) REFERENCES organismInformation(organismId) ON DELETE CASCADE;
-- lotInformation 1-1 taskpondinput
ALTER TABLE lotInformation ADD FOREIGN KEY(lotId) REFERENCES taskPondInput(lotId) ON DELETE CASCADE;
-- lotInformation 1-1 organismAmountInformation
ALTER TABLE lotInformation ADD FOREIGN KEY(lotId) REFERENCES organismAmountInformation(lotId) ON DELETE CASCADE;
-- lotInformation 1-1 samplingMeasurementInformation
ALTER TABLE lotInformation ADD FOREIGN KEY(lotId) REFERENCES samplingMeasurementInformation(lotId) ON DELETE CASCADE;
-- lotInformation 1-1 monitoringMeasurementInformation
ALTER TABLE lotInformation ADD FOREIGN KEY(lotId) REFERENCES monitoringMeasurementInformation(lotId) ON DELETE CASCADE;

-- organismInformation
-- organismInformation -1-1 stockSeedInformation
ALTER TABLE organismInformation ADD FOREIGN KEY(organismId) REFERENCES stockSeedInformation(masterOrganismId) ON DELETE CASCADE;
-- organismInformation -1-n thresholdInformation
ALTER TABLE thresholdInformation ADD FOREIGN KEY(organismId) REFERENCES organismInformation(organismId) ON DELETE CASCADE;

-- thresholdInformation
-- thresholdInformation -1-1 masterThresholdInformation
ALTER TABLE masterThresholdInformation ADD FOREIGN KEY(masterThresholdId) REFERENCES thresholdInformation(masterThresholdId) ON DELETE CASCADE;