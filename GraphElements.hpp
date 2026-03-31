#pragma once
#include <imgui_node_editor.h>
namespace ed = ax::NodeEditor;

enum class PinType {
    Input,
    Output
};

struct Pin {
    ed::PinId ID;
    PinType Type;

    Pin(int id, PinType type) : ID(id), Type(type) {}
};

struct Link {
    ed::LinkId ID;
    ed::PinId StartPinID;
    ed::PinId EndPinID;

    Link(int id, ed::PinId start, ed::PinId end) : ID(id), StartPinID(start), EndPinID(end) {}
};