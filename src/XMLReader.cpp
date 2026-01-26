#include "XMLReader.h"
#include <expat.h>
#include <queue>

struct CXMLReader::SImplementation{
    static void ExpatStartElement(void *data, const XML_Char *name, const XML_Char **attrs) {

    }
    
    static void ExpatEndElement(void *data, const XML_Char *name) {

    }

    static void ExpatCharacterData(void *data, const XML_Char *s, int len) {

    }

    SImplementation(std::shared_ptr<CDataSource> src) {

    }

    ~SImplementation() {

    }

    bool End() const {

    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        
    }
};

CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src){

}

CXMLReader::~CXMLReader(){

}

bool CXMLReader::End() const{

}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){
    return false;
}
