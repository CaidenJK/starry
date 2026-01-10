#include "Interface.h"

namespace Starry
{
    Interface::Interface()
    {
    }

    Interface::~Interface()
    {
    }

    void Interface::loadElement(std::shared_ptr<UIElement>& element, int drawOrder)
    {
        elements.emplace(drawOrder, element);
    }

    void Interface::Display()
    {
        for (auto element : elements) {
            element.second->Draw();
        }
    }
}