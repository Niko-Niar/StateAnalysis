//
// Created by kulakov on 13.04.2021.
//

#ifndef ORMODULE2_ORMODULEIMPL_H
#define ORMODULE2_ORMODULEIMPL_H

#include "brocker/IBrocker.h"
#include "database/IDatabase.h"
#include "ormodulecallback.h"

class ORModuleImpl : public IBrockerCallback, public IDatabaseCallback {

public:
    ~ORModuleImpl() override;

    void init(const std::string& moduleType, const std::string &service_id, ORModuleCallback *cb, const std::string &config_path);

    IBrocker * brocker() const { return m_brocker; }

    IDatabase *configDB() const { return m_configDB; }

    IDatabase *sourceDb() const { return m_sourceDB; }

    IDatabase *eventDb() const { return m_eventDB; }

    std::string getClusterName() const { return m_clusterName; }

    std::string getStorageDir() const {return m_storageDir; }

    std::string getSettingsPath() const;

    std::string getModuleType() const {return m_moduleType; }

    std::string serviceId() const { return m_serviceId; }

    ORModuleCallback *callback() const { return m_callback; }

protected:
    /**
     * чтение конфига и загрузка адаптеров
     */
    void loadAdapters();

    void brockerGotConfigMessage() override;

    void brockerGotAction(const std::string&, const std::string&) override;

    void brockerGotEvent(const std::string&, const std::string&) override;

    void databaseGotConfig(const std::string &guid, DataBlock *config) override;

    void databaseGotOtherConfigs(const std::string &dataType, const std::map<std::string, DataBlock *>& vector) override;

    void databaseGotError(IDatabase *instance, const std::string &errorMessage) override;

    void databaseGotData(DataQuery query, DataBlock *result) override;

    std::string m_serviceId{};
    IBrocker * m_brocker = nullptr;
    IDatabase * m_configDB = nullptr;
    IDatabase * m_sourceDB = nullptr;
    IDatabase * m_eventDB = nullptr;
    ORModuleCallback * m_callback = nullptr;
    std::string m_configPath{};
    std::string m_clusterName{};
    std::string m_moduleType{};
    std::string m_storageDir{};

    DataBlock *m_configBlock = nullptr;

    void addDefaults();
};


#endif //ORMODULE2_ORMODULEIMPL_H
