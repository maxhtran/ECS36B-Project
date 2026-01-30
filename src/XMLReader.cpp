#include "XMLReader.h"
#include <expat.h>
#include <queue>

struct CXMLReader::SImplementation{
    XML_Parser parser;
    std::queue<SXMLEntity> entityQueue;
    std::shared_ptr<CDataSource> dataSource;
    bool endParsing;

    static void ExpatStartElement(void *data, const XML_Char *name, const XML_Char **attrs) {
        SImplementation *userData = (SImplementation *)data;
        SXMLEntity entity;

        entity.DType = SXMLEntity::EType::StartElement;
        entity.DNameData = name;

        int i = 0;
        while (attrs[i] && attrs[i + 1]) {
            entity.DAttributes.push_back({attrs[i], attrs[i + 1]});
            i += 2;
        }

        userData->entityQueue.push(entity);
    }
    
    static void ExpatEndElement(void *data, const XML_Char *name) {
        SImplementation *userData = (SImplementation *)data;
        SXMLEntity entity;

        entity.DType = SXMLEntity::EType::EndElement;
        entity.DNameData = name;

        userData->entityQueue.push(entity);
    }

    static void ExpatCharacterData(void *data, const XML_Char *s, int len) {
        SImplementation *userData = (SImplementation *)data;

        if (userData->entityQueue.back().DType == SXMLEntity::EType::CharData) {
            userData->entityQueue.back().DNameData.append(s, len);
            return;
        }

        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::CharData;
        entity.DNameData.append(s, len);

        userData->entityQueue.push(entity);
    }

    SImplementation(std::shared_ptr<CDataSource> src) {
        parser = XML_ParserCreate(nullptr);
        XML_SetElementHandler(parser, ExpatStartElement, ExpatEndElement);
        XML_SetCharacterDataHandler(parser, ExpatCharacterData);
        XML_SetUserData(parser, this);
        dataSource = src;
        endParsing = false;
    }

    ~SImplementation() {
        XML_ParserFree(parser);
    }

    bool End() const {
        return entityQueue.empty();
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        std::vector<char> buffer;
        while (dataSource->Read(buffer, 512)) {
            if (XML_Parse(parser, buffer.data(), buffer.size(), 0) == XML_STATUS_ERROR) {
                break;
            }
        }
        XML_Parse(parser, nullptr, 0, 1);

        while (!entityQueue.empty()) {
            SXMLEntity entityToRead = entityQueue.front();
            entityQueue.pop();

            if (entityToRead.DType == SXMLEntity::EType::CharData && skipcdata) {
                continue;
            }

            entity = entityToRead;
            return true;
        }

        return false;
    }
};

CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src){
    DImplementation = std::make_unique<SImplementation>(src);
}

CXMLReader::~CXMLReader(){

}

bool CXMLReader::End() const{
    return DImplementation->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){
    return DImplementation->ReadEntity(entity, skipcdata);
}
