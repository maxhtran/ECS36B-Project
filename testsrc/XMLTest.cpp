#include <gtest/gtest.h>
#include "XMLReader.h"
#include "StringDataSource.h"

TEST(XMLReaderTest, SimpleTest){
    std::string testXML = "<tagname attr1=\"val1\"></tagname>";
    std::shared_ptr<CDataSource> dataSource = std::make_shared<CStringDataSource>(testXML);

    CXMLReader reader(dataSource);
    SXMLEntity entity;

    ASSERT_EQ(reader.ReadEntity(entity), true); // test <tagname attr1="val1">
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "tagname");
    EXPECT_EQ(entity.AttributeExists("attr1"), true);
    EXPECT_EQ(entity.AttributeValue("attr1"), "val1");

    ASSERT_EQ(reader.ReadEntity(entity), true); // test </tagname>
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "tagname");
    
    EXPECT_EQ(reader.End(), true);
}

TEST(XMLReaderTest, ElementTest){
    std::string testXML = "<parent attr1=\"val1\"><child attr2=\"val2\"></child></parent>";
    std::shared_ptr<CDataSource> dataSource = std::make_shared<CStringDataSource>(testXML);

    CXMLReader reader(dataSource);
    SXMLEntity entity;

    ASSERT_EQ(reader.ReadEntity(entity), true); // test <parent attr1="val1">
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "parent");
    EXPECT_EQ(entity.AttributeExists("attr1"), true);
    EXPECT_EQ(entity.AttributeValue("attr1"), "val1");

    ASSERT_EQ(reader.ReadEntity(entity), true); // test <child attr2="val2">
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "child");
    EXPECT_EQ(entity.AttributeExists("attr2"), true);
    EXPECT_EQ(entity.AttributeValue("attr2"), "val2");

    ASSERT_EQ(reader.ReadEntity(entity), true); // test </child>
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "child");

    ASSERT_EQ(reader.ReadEntity(entity), true); // test </parent>
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "parent");

    EXPECT_EQ(reader.End(), true);
}

TEST(XMLReaderTest, CDataTest){
    std::string testXML = "<tag>THIS IS THE CDATA</tag>";
    std::shared_ptr<CDataSource> dataSource = std::make_shared<CStringDataSource>(testXML);

    CXMLReader reader(dataSource);
    SXMLEntity entity;

    ASSERT_EQ(reader.ReadEntity(entity), true); // skip <tag>
    reader.ReadEntity(entity); // read character data
    EXPECT_EQ(entity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(entity.DNameData, "THIS IS THE CDATA");
}

TEST(XMLReaderTest, LongCDataTest){
    // 1000 characters
    std::string testXML = "<tag>Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. N</tag>";
    std::shared_ptr<CDataSource> dataSource = std::make_shared<CStringDataSource>(testXML);

    CXMLReader reader(dataSource);
    SXMLEntity entity;

    ASSERT_EQ(reader.ReadEntity(entity), true); // skip <tag>
    ASSERT_EQ(reader.ReadEntity(entity), true); // read character data
    EXPECT_EQ(entity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(entity.DNameData, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. N");
}

TEST(XMLReaderTest, SpecialCharacterTest){
    std::string testXML = "<test>1 &lt; x &gt; 5, &quot;where&apos;s &amp; theirs&quot;</test>";
    std::shared_ptr<CDataSource> dataSource = std::make_shared<CStringDataSource>(testXML);

    CXMLReader reader(dataSource);
    SXMLEntity entity;

    ASSERT_EQ(reader.ReadEntity(entity), true); // skip <tag>
    ASSERT_EQ(reader.ReadEntity(entity), true); // read character data
    EXPECT_EQ(entity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(entity.DNameData, "1 < x > 5, \"where's & theirs\"");
}

TEST(XMLReaderTest, InvalidXMLTest){
    // Test 1
    std::string testXML = "> <foo>";
    std::shared_ptr<CDataSource> dataSource = std::make_shared<CStringDataSource>(testXML);

    CXMLReader reader(dataSource);
    SXMLEntity entity;
    ASSERT_EQ(reader.ReadEntity(entity), false);

    // Test 2
    testXML = "<foo><bar></foo>";
    dataSource = std::make_shared<CStringDataSource>(testXML);

    CXMLReader reader2(dataSource);
    
    ASSERT_EQ(reader2.ReadEntity(entity), true);
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "foo");

    ASSERT_EQ(reader2.ReadEntity(entity), true);
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "bar");

    ASSERT_EQ(reader2.ReadEntity(entity), false); // should expect </bar>, not </foo>
}

TEST(XMLReaderTest, LongCharDataCrosses512Boundary){
    std::string upTo512 = "<tag>Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivm";
    std::string past512 = "us elem</tag>";
    std::string testXML = upTo512 + past512;
    std::shared_ptr<CDataSource> dataSource = std::make_shared<CStringDataSource>(testXML);

    CXMLReader reader(dataSource);
    SXMLEntity entity;

    ASSERT_EQ(reader.ReadEntity(entity), true); // skip <tag>
    ASSERT_EQ(reader.ReadEntity(entity), true); // read character data
    EXPECT_EQ(entity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(entity.DNameData, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivmus elem");
}