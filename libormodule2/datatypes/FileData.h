//
// Created by kulakov on 13.04.2021.
//

#ifndef ORMODULE2_FILEDATA_H
#define ORMODULE2_FILEDATA_H

#include "../DataBlock.h"

#include <string>

class FileData : public DataBlock {
public:
    FileData() = default;
    FileData(const std::string &type, const std::string &result) {
        setValue("type", type);
        setValue("result", result);
    }

    void update(const std::string &type, const std::string &result, const std::string &content) {
        setValue("type", type);
        setValue("result", result);
        setValue("content", content);
    }

protected:
    std::map<std::string, OR_DATA_TYPES> getBlockDescription() const override {
        return m_items;
    }

    std::map<std::string, OR_DATA_TYPES> m_items = {
            {"type", OR_TYPE_STRING},
            {"result", OR_TYPE_STRING},
            {"content", OR_TYPE_STRING}};
};

class FilePathData : public DataBlock {
public:
    FilePathData() = default;
    explicit FilePathData(const std::string &file) {
        setValue("file", file);
    }

protected:
    std::map<std::string, OR_DATA_TYPES> getBlockDescription() const override {
        return m_items;
    }

    std::map<std::string, OR_DATA_TYPES> m_items = {
            {"file", OR_TYPE_STRING}
    };
};

class FileRequestData : public DataBlock {
public:
    FileRequestData() = default;
    FileRequestData(const std::string &sender, const int duration) {
        setValue("sender", sender);
        setValue("record_duration", duration);
    }
protected:
    std::map<std::string, OR_DATA_TYPES> getBlockDescription() const override {
        return m_items;
    }

    std::map<std::string, OR_DATA_TYPES> m_items = {
            {"sender", OR_TYPE_STRING},
            {"record_duration", OR_TYPE_INT}
    };
};

#endif //ORMODULE2_FILEDATA_H
