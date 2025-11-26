#pragma once
#include "VoxelEngine/Core/Core.h"
#include "Layer.h"

#include <vector>
namespace VoxelEngine {
class VE_API LayerStack {
public:
    LayerStack() { }
    ~LayerStack() { }

    template <typename T>
        requires(std::is_base_of_v<Layer, T>)
    void PushLayer()
    {
        Scope<Layer> layer = std::make_unique<T>();
        layer->OnAttach();
        m_Layers.push_back(std::move(layer));
    }
    template <typename T>
        requires(std::is_base_of_v<Layer, T>)
    void PushOverlay()
    {
    }
    template <typename T>
        requires(std::is_base_of_v<Layer, T>)
    void PopLayer()
    {
    }
    template <typename T>
        requires(std::is_base_of_v<Layer, T>)
    void PopOverlay()
    {
    }
    template <typename T>
        requires(std::is_base_of_v<Layer, T>)
    T* GetLayer()
    {
        for (const auto& layer : m_Layers) {
            if (auto casted = dynamic_cast<T*>(layer.get())) {
                return casted;
            }
        }
        return nullptr;
    }

    std::vector<Scope<Layer>>::iterator begin() { return m_Layers.begin(); }
    std::vector<Scope<Layer>>::iterator end() { return m_Layers.end(); }

private:
    std::vector<Scope<Layer>> m_Layers;
    unsigned int m_LayerInsertIndex = 0;
};

} // namespace VoxelEngine
