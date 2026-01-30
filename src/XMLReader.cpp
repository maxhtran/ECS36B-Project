#include "XMLReader.h"
#include <expat.h>
#include <queue>

struct CXMLReader::SImplementation{
    XML_Parser parser;
    std::queue<SXMLEntity> entityQueue;
    std::shared_ptr<CDataSource> dataSource;

    static void ExpatStartElement(void *data, const XML_Char *name, const XML_Char **attrs) {
        SImplementation *userData = (SImplementation *)data; // allows this static function to access the member data of the current object
        SXMLEntity entity; // declare entity to push

        // fill out entity information
        entity.DType = SXMLEntity::EType::StartElement;
        entity.DNameData = name;

        // add attributes to DAttributes vector of StartElement entity
        int i = 0;
        while (attrs[i] && attrs[i + 1]) {
            entity.DAttributes.push_back({attrs[i], attrs[i + 1]});
            i += 2;
        }

        userData->entityQueue.push(entity); // push entity to queue
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

        // deals with CharData longer than buffer size of 512 characters
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
        // initialize member data
        parser = XML_ParserCreate(nullptr);
        XML_SetElementHandler(parser, ExpatStartElement, ExpatEndElement);
        XML_SetCharacterDataHandler(parser, ExpatCharacterData);
        XML_SetUserData(parser, this);
        dataSource = src;
    }

    ~SImplementation() {
        XML_ParserFree(parser); // delete/free XML parser when done
    }

    bool End() const {
        return entityQueue.empty(); // no more XML text to parse + no more XML entities to read = donea
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        std::vector<char> buffer;
        while (dataSource->Read(buffer, 512)) { // while XML can be read from the data source, parse it
            if (XML_Parse(parser, buffer.data(), buffer.size(), 0) == XML_STATUS_ERROR) {
                break; // if parser encounters error, just move on
            }
        }
        XML_Parse(parser, nullptr, 0, 1); // final call to XML parser once all XML has been read

        while (!entityQueue.empty()) { // while there are entities in the queue, read them
            SXMLEntity entityToRead = entityQueue.front();
            entityQueue.pop();

            if (entityToRead.DType == SXMLEntity::EType::CharData && skipcdata) {
                continue; // skip CharData entities if skipcdata is true
            }

            entity = entityToRead;
            return true;
        }

        return false; // no more XML text to parse + no more XML entities to read = done
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
