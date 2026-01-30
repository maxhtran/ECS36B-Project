#include <gtest/gtest.h>
#include "SVGWriter.h"
#include "StringDataSink.h"

TEST(SVGWriterTest, CreateDestroyTest){ // Verifies SVG header and footer are written on construction/destruction
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        CSVGWriter Writer(Sink, 100, 50);
    }
    EXPECT_EQ(Sink->String(),  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                "<svg width=\"100\" height=\"50\" xmlns=\"http://www.w3.org/2000/svg\">\n"
                                "</svg>");
}

TEST(SVGWriterTest, CircleTest){ // Verifies circle is written correctly
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        SSVGPoint center = {5,5};
        TSVGReal radius = 1.0;
        TAttribute attr1 = {"fill", "none"};
        TAttribute attr2 = {"stroke", "blue"};
        TAttribute attr3 = {"opacity", "0.5"};
        TAttributes style = {attr1, attr2, attr3};
        CSVGWriter Writer(Sink, 100, 50);
        EXPECT_TRUE(Writer.Circle(center, radius, style));
    }
    EXPECT_EQ(Sink->String(),"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                            "<svg width=\"100\" height=\"50\" xmlns=\"http://www.w3.org/2000/svg\">\n"
                            "<circle cx=\"5.000000\" cy=\"5.000000\" r=\"1.000000\" style=\"fill:none;stroke:blue;opacity:0.5\"/>\n"
                            "</svg>");
}

TEST(SVGWriterTest, RectangleTest){  // Verifies rectangle element is written correctly
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        SSVGPoint point = {1,1};
        SSVGSize size = {1,2};
        TAttribute attr1 = {"fill", "red"};
        TAttribute attr2 = {"stroke", "green"};
        TAttribute attr3 = {"stroke-width", "2"};
        TAttributes style = {attr1, attr2, attr3};
        CSVGWriter Writer(Sink, 100, 50);
        EXPECT_TRUE(Writer.Rectange(point, size, style));
    }
    EXPECT_EQ(Sink->String(),"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                            "<svg width=\"100\" height=\"50\" xmlns=\"http://www.w3.org/2000/svg\">\n"
                            "<rect x=\"1.000000\" y=\"1.000000\" width=\"1.000000\" height=\"2.000000\" style=\"fill:red;stroke:green;stroke-width:2\"/>\n"
                            "</svg>");
}

TEST(SVGWriterTest, LineTest){ // Verifies line element is written correctly
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        SSVGPoint start = {1,1};
        SSVGPoint end = {2,2};
        TAttribute attr1 = {"fill", "none"};
        TAttribute attr2 = {"stroke", "blue"};
        TAttribute attr3 = {"stroke-width", "1"};
        TAttributes style = {attr1, attr2, attr3};
        CSVGWriter Writer(Sink, 100, 50);
        EXPECT_TRUE(Writer.Line(start, end, style));
    }
    EXPECT_EQ(Sink->String(),"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                            "<svg width=\"100\" height=\"50\" xmlns=\"http://www.w3.org/2000/svg\">\n"
                            "<line x1=\"1.000000\" y1=\"1.000000\" x2=\"2.000000\" y2=\"2.000000\" style=\"fill:none;stroke:blue;stroke-width:1\"/>\n"
                            "</svg>");
}

TEST(SVGWriterTest, SimplePathTest){ // Verifies path is written correctly
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        SSVGPoint point1 = {1,1};
        SSVGPoint point2 = {2,2};
        SSVGPoint point3 = {3,4};
        std::vector<SSVGPoint> points = {point1, point2, point3};
        TAttribute attr1 = {"fill", "none"};
        TAttribute attr2 = {"stroke", "red"};
        TAttribute attr3 = {"stroke-width", "1"};
        TAttributes style = {attr1, attr2, attr3};
        CSVGWriter Writer(Sink, 100, 50);
        EXPECT_TRUE(Writer.SimplePath(points, style));
    }
    EXPECT_EQ(Sink->String(), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                            "<svg width=\"100\" height=\"50\" xmlns=\"http://www.w3.org/2000/svg\">\n"
                            "<line x1=\"1.000000\" y1=\"1.000000\" x2=\"2.000000\" y2=\"2.000000\" style=\"fill:none;stroke:red;stroke-width:1\"/>\n"
                            "<line x1=\"2.000000\" y1=\"2.000000\" x2=\"3.000000\" y2=\"4.000000\" style=\"fill:none;stroke:red;stroke-width:1\"/>\n"
                            "</svg>");
}

TEST(SVGWriterTest, GroupTest){ // Verifies group begin/end wrapping
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    {
        SSVGPoint center = {5,5};
        TSVGReal radius = 1.0;
        TAttribute attr1 = {"fill", "none"};
        TAttribute attr2 = {"stroke", "red"};
        TAttribute attr3 = {"stroke-width", "1"};
        TAttributes style = {attr1, attr2, attr3};
        CSVGWriter Writer(Sink, 100, 50);
        EXPECT_TRUE(Writer.GroupBegin(style));
        EXPECT_TRUE(Writer.Circle(center, radius, TAttributes{}));
        EXPECT_TRUE(Writer.GroupEnd());
    }
    EXPECT_EQ(Sink->String(), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                            "<svg width=\"100\" height=\"50\" xmlns=\"http://www.w3.org/2000/svg\">\n"
                            "<g style=\"fill:none;stroke:red;stroke-width:1\">\n"
                            "<circle cx=\"5.000000\" cy=\"5.000000\" r=\"1.000000\" style=\"\"/>\n"
                            "</g>\n"
                            "</svg>");
}

class CFailingSink : public CDataSink{
    public:
        int DValidCalls = 0;
        virtual ~CFailingSink(){};
        bool Put(const char &ch) noexcept override{
            if(DValidCalls){
                DValidCalls--;
                return true;
            }
            return false;
        }

        bool Write(const std::vector<char> &buf) noexcept override{
            if(DValidCalls){
                DValidCalls--;
                return true;
            }
            return false;
        }
};

TEST(SVGWriterTest, ErrorTests){ // Verifies failure handling for I/O errors, invalid parameters, and invalid group state
    SSVGPoint center = {5,5};
    TSVGReal radius = 1.0;
    SSVGPoint point1 = {1,1};
    SSVGPoint point2 = {2,2};
    SSVGSize size = {1,2};
    TAttribute attr1 = {"fill", "none"};
    TAttribute attr2 = {"stroke", "red"};
    TAttributes groupStyle = {attr1, attr2};

    {
        std::shared_ptr<CFailingSink> Sink = std::make_shared<CFailingSink>();
        Sink->DValidCalls = 65536;
        CSVGWriter Writer(Sink, 100, 50);
        Sink->DValidCalls = 0;
        EXPECT_FALSE(Writer.Circle(center, radius, TAttributes{}));
    }
    {
        std::shared_ptr<CFailingSink> Sink = std::make_shared<CFailingSink>();
        Sink->DValidCalls = 65536;
        CSVGWriter Writer(Sink, 100, 50);
        Sink->DValidCalls = 0;
        EXPECT_FALSE(Writer.Rectange(point1, size, TAttributes{}));
    }
    {
        std::shared_ptr<CFailingSink> Sink = std::make_shared<CFailingSink>();
        Sink->DValidCalls = 65536;
        CSVGWriter Writer(Sink, 100, 50);
        Sink->DValidCalls = 0;
        EXPECT_FALSE(Writer.Line(point1, point2, TAttributes{}));
    }
    {
        std::shared_ptr<CFailingSink> Sink = std::make_shared<CFailingSink>();
        Sink->DValidCalls = 65536;
        CSVGWriter Writer(Sink, 100, 50);
        Sink->DValidCalls = 0;
        EXPECT_FALSE(Writer.GroupBegin(groupStyle));
    }
    {
        std::shared_ptr<CFailingSink> Sink = std::make_shared<CFailingSink>();
        Sink->DValidCalls = 65536;
        CSVGWriter Writer(Sink, 100, 50);
        Sink->DValidCalls = 65536;
        EXPECT_TRUE(Writer.GroupBegin(groupStyle));
        Sink->DValidCalls = 0;
        EXPECT_FALSE(Writer.GroupEnd());
    }
    {
        std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
        CSVGWriter Writer(Sink, 100, 50);

        EXPECT_FALSE(Writer.Circle(center, 0.0, TAttributes{}));
        EXPECT_FALSE(Writer.Circle(center, -1.0, TAttributes{}));

        EXPECT_FALSE(Writer.Rectange(point1, SSVGSize{0, 2}, TAttributes{}));
        EXPECT_FALSE(Writer.Rectange(point1, SSVGSize{1, 0}, TAttributes{}));
        EXPECT_FALSE(Writer.Rectange(point1, SSVGSize{-1, 2}, TAttributes{}));
        EXPECT_FALSE(Writer.Rectange(point1, SSVGSize{1, -2}, TAttributes{}));

        EXPECT_FALSE(Writer.SimplePath({}, TAttributes{}));
        EXPECT_FALSE(Writer.SimplePath({point1}, TAttributes{}));

        EXPECT_FALSE(Writer.GroupEnd());
    }
}
