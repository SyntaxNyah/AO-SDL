#pragma once

#include "ui/IWidget.h"

#include "render/Texture.h"

#include <functional>
#include <map>
#include <optional>
#include <string>

/// Displays the online player list (PR/PU packets) with area filtering.
class PlayerListWidget : public IWidget {
  public:
    using IconLoader = std::function<std::optional<Texture2D>(const std::string& character)>;

    void handle_events() override;
    void render() override;

    void set_icon_loader(IconLoader loader);

  private:
    const Texture2D* resolve_icon(const std::string& character);

    bool filter_by_area_ = true;

    IconLoader icon_loader_;
    std::map<std::string, Texture2D> icon_cache_;
    std::map<std::string, double> icon_attempt_;
};
