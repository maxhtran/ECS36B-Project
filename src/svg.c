/**
 * @file svg.c
 * @brief Implementation of simple SVG drawing interface.
 *
 * Implements the basic functions for creating SVG documents.
 */
#include "svg.h"
#include <stdio.h>
#include <stdlib.h>


/**
 * @brief Opaque SVG drawing context.
 *
 * Holds the necessary data to implement functions.
 */
struct SVG_CONTEXT{
    svg_write_fn write_fn;
    svg_cleanup_fn cleanup_fn;
    svg_user_context_ptr user;
};

int numGroups = 0;

svg_context_ptr svg_create(svg_write_fn write_fn, 
                           svg_cleanup_fn cleanup_fn, 
                           svg_user_context_ptr user, 
                           svg_px_t width, 
                           svg_px_t height){
    if (!write_fn || !cleanup_fn || !user || width == 0 || height == 0) { return NULL; }
    
    svg_context_ptr context = (svg_context_ptr)malloc(sizeof(svg_context_t));
    context->write_fn = write_fn;
    context->cleanup_fn = cleanup_fn;
    context->user = user;

    
    char Fbuff[200];
    snprintf(Fbuff, sizeof(Fbuff), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\">\n", width, height);

    context->write_fn(context->user, Fbuff);

    return context;
}

svg_return_t svg_destroy(svg_context_ptr context){
    if (!context) { return SVG_ERR_NULL; }
    if (numGroups != 0) { return SVG_ERR_STATE; }
    
    context->write_fn(context->user, "</svg>");
    context->cleanup_fn(context->user);

    free(context);
    return SVG_OK;
}

svg_return_t svg_circle(svg_context_ptr context,
                        const svg_point_t *center,
                        svg_real_t radius,
                        const char *style){
    if (!context || !center) { return SVG_ERR_NULL; }

    char Fbuff[200];
    int charCheck = snprintf(Fbuff, sizeof(Fbuff), "<circle cx=\"%lf\" cy=\"%lf\" r=\"%lf\" style=\"%s\"/>\n", center->x, center->y, radius, style);

    if (charCheck >= 200 || radius <= 0) {
        return SVG_ERR_INVALID_ARG;
    }
    else {
        svg_return_t returnCode = context->write_fn(context->user, Fbuff);
        return returnCode;
    }
}


svg_return_t svg_rect(svg_context_ptr context,
                      const svg_point_t *top_left,
                      const svg_size_t *size,
                      const char* style){
    if (!context || !top_left || !size) { return SVG_ERR_NULL; }

    char Fbuff[200];
    int charCheck = snprintf(Fbuff, sizeof(Fbuff), "<rect x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" style=\"%s\"/>\n", top_left->x, top_left->y, size->width, size->height, style);

    if (charCheck >= 200 || size->width <= 0 || size->height <= 0) {
        return SVG_ERR_INVALID_ARG;
    }
    else {
        svg_return_t returnCode = context->write_fn(context->user, Fbuff);
        return returnCode;
    }
}

svg_return_t svg_line(svg_context_ptr context,
                      const svg_point_t *start,
                      const svg_point_t *end,
                      const char* style){
    if (!context || !start || !end) { return SVG_ERR_NULL; }

    char Fbuff[200];
    int charCheck = snprintf(Fbuff, sizeof(Fbuff), "<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" style=\"%s\"/>\n", start->x, start->y, end->x, end->y, style);

    if (charCheck >= 200) {
        return SVG_ERR_INVALID_ARG;
    }
    else {
        svg_return_t returnCode = context->write_fn(context->user, Fbuff);
        return returnCode;
    }
}

svg_return_t svg_group_begin(svg_context_ptr context, 
                             const char* attrs){
    if (!context) { return SVG_ERR_NULL; }

    if (!attrs) {
        svg_return_t returnCode = context->write_fn(context->user, "<g>");
        numGroups++;
        return returnCode;
    }
    else {
        char Fbuff[200];
        int charCheck = snprintf(Fbuff, sizeof(Fbuff), "<g style=\"%s\">\n", attrs);

        if (charCheck >= 200) {
            return SVG_ERR_INVALID_ARG;
        }
        else {
            svg_return_t returnCode = context->write_fn(context->user, Fbuff);
            numGroups++;
            return returnCode;
        }
    }
}

svg_return_t svg_group_end(svg_context_ptr context){
    if (!context) { return SVG_ERR_NULL; }

    svg_return_t returnCode = context->write_fn(context->user, "</g>\n");
    numGroups--;
    return returnCode;
}
