mx, my = 0,0
mouse_delta = 0
local vx, vy, vz
local k = 1.5
local plane_size = 2
local max_depth = 8


function QuadTree(x,y, color)
    local result = {}

    result.x = x
    result.y = y
    result.color = color
    result.children = {}
    return result

end

function draw_plane(x, y, z, size, color) 
    io.write(
    "\"ox\": "..x..",".. 
    "\"oy\": "..z..","..
    "\"size\": "..size
    )
  --[[
    gh_object.set_vertices_color(mesh_id, color.r, color.g, color.b, color.a)
    gh_object.set_position(mesh_plane, x, y, z)
    gh_object.set_scale(mesh_plane, size, size, size)
    gh_gpu_program.bind(simple_prog)
    gh_gpu_program.uniform4f(simple_prog, "u_color", color.r, color.g, color.b, color.a)
    gh_object.render(mesh_plane)
    ]]

end

function get_quad_by_index(i)
    local result = {}
    if i == 0 then
        result.x = -1;
        result.y = -1;
        result.color = lbc
    elseif i == 1 then
        result.x = -1;
        result.y = 1;
        result.color = ltc
    elseif i == 2 then
        result.x = 1;
        result.y = -1;
        result.color = rtc
    elseif i == 3 then
        result.x = 1;
        result.y = 1;
        result.color = rbc
    end
    return result
end

function get_index_by_position(ox, oy, px, py)
    if px < ox then 
        if py < oy then
            return 0
        else
            return 1
        end
    else
        if py < oy then
            return 2
        else
            return 3
        end
    end
end

function get_offset_by_index(i)
    local ox, oy 
    if i == 0 then
        ox = -1;
        oy = -1;
    elseif i == 1 then
        ox = -1;
        oy = 1;
    elseif i == 2 then
        ox = 1;
        oy = -1;
    elseif i == 3 then
        ox = 1;
        oy = 1;
    end
    return ox, oy
end

function get_node_size(parent_node_size)
    return (0.5*parent_node_size)
end

function get_origin(pox, poy, n, parent_node_size)
    local ox, oy = get_offset_by_index(n)
    return 
        pox + 0.5*get_node_size(parent_node_size)*ox, 
        poy + 0.5*get_node_size(parent_node_size)*oy
end

function need_split(depth, x, y, ox, oy, L)
    if depth < max_depth then
        local d = math.max(
            math.min(math.abs(x-ox), math.abs(x-ox-L)), 
            math.min(math.abs(y-oy), math.abs(y-oy-L)),
            math.min(0,0)
        )
        return d < k*L
        --return true
    end
    return false
end

function build_quadtree(qt, depth, px, py, size)
    if need_split(depth, px, py, qt.x - size*0.5, qt.y - size*0.5, size) then
        for i = 0, 3 do
            local q = get_quad_by_index(i)
            pcx, pcy = get_origin(qt.x, qt.y, i, size)
            qt.children[i] = QuadTree(pcx, pcy, q.color)

            build_quadtree(
                qt.children[i], depth + 1, px, py, size*0.5
            )
            --
            --
        end
    end
end

function draw_quadtree(qt, ox, oy, size, level, last)
    if qt ~= nil then
        if #qt.children == 0 then
          io.write(depth_space(level).."{ ")
          draw_plane(qt.x,0,qt.y, size, qt.color)
          if last then
            io.write(" }")
          else
            io.write(" },")
          end
          print("")
        else

            io.write(depth_space(level).." [\n")
            for i = 0, 3 do
                offset_x, offset_y = get_offset_by_index(i)
                local nox, noy = get_origin(ox, oy, i, size)
                draw_quadtree(qt.children[i], nox, noy, get_node_size(size), level + 1, i==3) 
            end
            if last then
              io.write(depth_space(level).." ]\n")
            else
              io.write(depth_space(level).." ],\n")
            end
        end
    end
end

function depth_space(n)
  local spaces = ""
  for i = 1, n do
    spaces=spaces.." "
  end
  return spaces
end

function render()
    local px, pz = 0.5, 0.5
    qx, qz = px, pz
    local qt = QuadTree(0, 0, lbc)
    build_quadtree(qt, 0, qx, qz, plane_size)

    
    local level = 0
    print(depth_space(level).."{")
    print(depth_space(level + 1)..[["root": ]].."")
    draw_quadtree(qt, 0, 0, plane_size, level, true)
    print(depth_space(level).."}")
end

render()
