/**
 * @file   LegalizationDBUtils.h
 * @author Yibo Lin
 * @date   Nov 2019
 */

#ifndef _DREAMPLACE_UTILITY_LEGALIZATIONDBUTILS_H
#define _DREAMPLACE_UTILITY_LEGALIZATIONDBUTILS_H

DREAMPLACE_BEGIN_NAMESPACE

/// @brief make a database for detailed placement. 
/// Only movable nodes will be moved. Fixed nodes and filler nodes are fixed. 
/// 
/// @param init_pos initial x/y location of nodes, including movable nodes, fixed nodes, and filler nodes, [0, num_movable_nodes) are movable nodes, [num_movable_nodes, num_nodes-num_filler_nodes) are fixed nodes, [num_nodes-num_filler_nodes, num_nodes) are filler nodes. x values are at the first half of the array, y values are at the second half of the array
/// @param pos x/y locations to write 
/// @param node_size_x width of nodes, including movable nodes, fixed nodes, and filler nodes, [0, num_movable_nodes) are movable nodes, [num_movable_nodes, num_nodes-num_filler_nodes) are fixed nodes, [num_nodes-num_filler_nodes, num_nodes) are filler nodes
/// @param node_size_y height of nodes, including movable nodes, fixed nodes, and filler nodes, same as node_size_x
/// @param xl left edge of bounding box of layout area 
/// @param yl bottom edge of bounding box of layout area 
/// @param xh right edge of bounding box of layout area 
/// @param yh top edge of bounding box of layout area 
/// @param site_width width of a placement site 
/// @param row_height height of a placement row 
/// @param num_bins_x number of bins in horizontal direction 
/// @param num_bins_y number of bins in vertical direction 
/// @param num_movable_nodes number of movable nodes, movable nodes are in the range of [0, num_movable_nodes)
/// @param number of filler nodes, filler nodes are in the range of [num_nodes-num_filler_nodes, num_nodes)
template <typename T>
LegalizationDB<T> make_placedb(
        at::Tensor init_pos,
        at::Tensor pos, 
        at::Tensor node_size_x,
        at::Tensor node_size_y,
        at::Tensor flat_region_boxes, 
        at::Tensor flat_region_boxes_start, 
        at::Tensor node2fence_region_map, 
        double xl, 
        double yl, 
        double xh, 
        double yh, 
        double site_width, double row_height, 
        int num_bins_x, 
        int num_bins_y,
        int num_movable_nodes, 
        int num_terminal_NIs, 
        int num_filler_nodes
        )
{
    LegalizationDB<T> db; 
    int num_nodes = init_pos.numel()/2;

    db.init_x = init_pos.data<T>(); 
    db.init_y = init_pos.data<T>()+num_nodes; 
    db.node_size_x = node_size_x.data<T>(); 
    db.node_size_y = node_size_y.data<T>(); 
    db.flat_region_boxes = flat_region_boxes.data<T>();
    db.flat_region_boxes_start = flat_region_boxes_start.data<int>();
    db.node2fence_region_map = node2fence_region_map.data<int>();
    db.x = pos.data<T>(); 
    db.y = pos.data<T>()+num_nodes; 
    db.xl = xl; 
    db.yl = yl; 
    db.xh = xh; 
    db.yh = yh; 
    db.site_width = site_width; 
    db.row_height = row_height; 
    db.bin_size_x = (xh-xl)/num_bins_x; 
    db.bin_size_y = (yh-yl)/num_bins_y; 
    db.num_bins_x = num_bins_x; 
    db.num_bins_y = num_bins_y; 
    db.num_sites_x = (xh-xl)/site_width; 
    db.num_sites_y = (yh-yl)/row_height; 
    // ignore fillers and terminal_NIs 
    db.num_nodes = num_nodes - num_filler_nodes - num_terminal_NIs; 
    db.num_movable_nodes = num_movable_nodes; 
    db.num_regions = flat_region_boxes_start.numel()-1;

    return db; 
}

DREAMPLACE_END_NAMESPACE

#endif
