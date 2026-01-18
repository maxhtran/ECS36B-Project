#include "svg.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

// Helper structure to capture written SVG strings
struct STestOutput{
    std::vector<std::string> DLines;
    bool DDestroyed = false;

    std::string JoinOutput(){
        std::string Result;

        for (const auto& line : DLines){
            Result += line;
        }
        return Result;
    }
};

// Callback to capture SVG output
svg_return_t write_callback(svg_user_context_ptr user, const char* text){
    if(!user || !text){
        return SVG_ERR_NULL;
    }
    STestOutput* OutPtr = static_cast<STestOutput*>(user);
    OutPtr->DLines.push_back(text);
    return SVG_OK;
}

// Cleanup callback (just returns OK for testing)
svg_return_t cleanup_callback(svg_user_context_ptr user){
    if(!user){
        return SVG_ERR_NULL;
    }
    STestOutput* OutPtr = static_cast<STestOutput*>(user);
    if(OutPtr->DDestroyed){
        return SVG_ERR_STATE;
    }
    OutPtr->DDestroyed = true;
    return SVG_OK;
}

// --- TEST FIXTURE ---
class SVGTest : public ::testing::Test{
    protected:
        STestOutput DOutput;
        svg_context_ptr DContext = nullptr;

        void SetUp() override{
            DContext = svg_create(write_callback, cleanup_callback, &DOutput, 100, 100);
            ASSERT_NE(DContext, nullptr);
        }

        void TearDown() override{
            if(DContext){
                svg_destroy(DContext);
                DContext = nullptr;
            }
        }
};

// --- BASIC CREATION TEST ---
TEST_F(SVGTest, CreateAndDestroy){
    svg_context_ptr context = svg_create(write_callback,
                                         cleanup_callback,
                                         &DOutput,
                                         100,
                                         100);
    EXPECT_NE(context,nullptr);
    EXPECT_EQ(svg_destroy(context),SVG_OK);
}

// --- INVALID INPUT TESTS ---
TEST_F(SVGTest, NullContextFunctions){
    svg_point_t center = {50, 50};

    svg_point_t corner = {25, 25};
    svg_size_t size = {45, 50};

    svg_point_t start = {20, 20};
    svg_point_t end = {80, 80};
    
    EXPECT_EQ(svg_circle(nullptr, &center, 45, "fill:red"), SVG_ERR_NULL);
    EXPECT_EQ(svg_rect(nullptr, &corner, &size, "fill:green"), SVG_ERR_NULL);
    EXPECT_EQ(svg_line(nullptr, &start, &end, "stroke:blue"), SVG_ERR_NULL);
}

// --- DRAWING TESTS ---
TEST_F(SVGTest, Circle){
    svg_context_ptr context = svg_create(write_callback,
                                         cleanup_callback,
                                         &DOutput,
                                         100,
                                         100);
    
    svg_point_t center = {50, 50};
    EXPECT_EQ(svg_circle(context, &center, 45, "fill:none; stroke:green; stroke-width:2"), SVG_OK);
    EXPECT_EQ(svg_circle(context, &center, 45, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec qu"), SVG_ERR_INVALID_ARG);

    svg_destroy(context);
}

TEST_F(SVGTest, Rectangle){
    svg_context_ptr context = svg_create(write_callback,
                                         cleanup_callback,
                                         &DOutput,
                                         100,
                                         100);
    
    svg_point_t corner = {25, 25};
    svg_size_t size = {45, 50};
    EXPECT_EQ(svg_rect(context, &corner, &size, "fill:none; stroke:blue; stroke-width:3"), SVG_OK);
    EXPECT_EQ(svg_rect(context, &corner, &size, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec qu"), SVG_ERR_INVALID_ARG);
    
    svg_destroy(context);
}

TEST_F(SVGTest, Line){
    svg_context_ptr context = svg_create(write_callback,
                                         cleanup_callback,
                                         &DOutput,
                                         100,
                                         100);

    svg_point_t start = {20, 20};
    svg_point_t end = {80, 80};
    EXPECT_EQ(svg_line(context, &start, &end, "stroke: red; stroke-width: 1"), SVG_OK);
    EXPECT_EQ(svg_line(context, &start, &end, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec qu"), SVG_ERR_INVALID_ARG);

    svg_destroy(context);
}

// --- GROUPING TEST ---
TEST_F(SVGTest, Grouping){
    svg_context_ptr context = svg_create(write_callback,
                                         cleanup_callback,
                                         &DOutput,
                                         100,
                                         100);

    EXPECT_EQ(svg_group_begin(context, "style=\"stroke:yellow; stroke-width:5\""), SVG_OK);
    EXPECT_EQ(svg_group_begin(context, nullptr), SVG_OK);
    EXPECT_EQ(svg_group_end(context), SVG_OK);

    EXPECT_EQ(svg_group_begin(nullptr, "style=\"stroke:yellow; stroke-width:5\""), SVG_ERR_NULL);
    EXPECT_EQ(svg_group_begin(context, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec qu"), SVG_ERR_INVALID_ARG);
    EXPECT_EQ(svg_group_end(nullptr), SVG_ERR_NULL);

    svg_destroy(context);
}

// --- EDGE CASES ---
TEST_F(SVGTest, ZeroDimensions){
    svg_context_ptr context = svg_create(write_callback,
                                         cleanup_callback,
                                         &DOutput,
                                         100,
                                         100);

    svg_point_t center = {50, 50};                          
    EXPECT_EQ(svg_circle(context, &center, 0, "fill:white; stroke:orange; stroke-width:2"), SVG_ERR_INVALID_ARG);

    svg_point_t corner = {25, 25};
    svg_size_t size = {0, 0};
    EXPECT_EQ(svg_rect(context, &corner, &size, "fill:blue; stroke:red; stroke-width:4"), SVG_ERR_INVALID_ARG);

    svg_point_t start = {0, 0};
    svg_point_t end = {0, 0};
    EXPECT_EQ(svg_line(context, &start, &end, "stroke:purple; stroke-width:2"), SVG_OK);

    svg_destroy(context);
}

TEST_F(SVGTest, NullPointPointer){
    svg_context_ptr context = svg_create(write_callback,
                                         cleanup_callback,
                                         &DOutput,
                                         100,
                                         100);

    EXPECT_EQ(svg_circle(context, nullptr, 10, "fill:orange; stroke:white; stroke-width:3"), SVG_ERR_NULL);
    EXPECT_EQ(svg_rect(context, nullptr, nullptr, "fill:purple; stroke:yellow; stroke-width:1"), SVG_ERR_NULL);
    EXPECT_EQ(svg_line(context, nullptr, nullptr, "stroke:green; stroke-width:2"), SVG_ERR_NULL);

    svg_destroy(context);
}

TEST_F(SVGTest, CreateEdgeCases){
    // Attempt to pass NULL callback functions
    svg_context_ptr context = svg_create(NULL, NULL, &DOutput, 100, 100);
    EXPECT_EQ(context, nullptr);

    // Attempt to pass NULL user pointer
    context = svg_create(write_callback, cleanup_callback, NULL, 100, 100);
    EXPECT_EQ(context, nullptr);

    // Attempt to create a context with invalid dimensions
    context = svg_create(write_callback, cleanup_callback, &DOutput, 0, 0);
    EXPECT_EQ(context, nullptr);
}

TEST_F(SVGTest, DestroyEdgeCases){
    // Attempt to destroy a NULL pointer (e.g. calling svg_destroy)
    EXPECT_EQ(svg_destroy(nullptr), SVG_ERR_NULL);

    // Attempt to destroy context with an unclosed group
    svg_context_ptr context = svg_create(write_callback, cleanup_callback, &DOutput, 100, 100);

    svg_group_begin(context, "style=\"stroke:yellow; stroke-width:5\"");
    EXPECT_EQ(svg_destroy(context), SVG_ERR_STATE);
}

// Callback to capture SVG output
svg_return_t write_error_callback(svg_user_context_ptr user, const char* text){
    int *FailureCount = (int *)user;
    if(*FailureCount){
        (*FailureCount)--;
        return SVG_OK;    
    }
    return SVG_ERR_IO;
}

// --- IO Errors ---
TEST_F(SVGTest, IOErrorTest){
    // Used professor's code from Piazza
    
    int SuccessCount = 10;
    svg_context_ptr context = svg_create(write_error_callback, cleanup_callback, &SuccessCount, 100, 100);
    svg_point_t center = {50, 50};

    SuccessCount = 0;
    EXPECT_EQ(svg_circle(context, &center, 2.0, NULL), SVG_ERR_IO);
}