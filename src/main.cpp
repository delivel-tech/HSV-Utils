#include <Geode/Geode.hpp>
#include <Geode/modify/ConfigureHSVWidget.hpp>
#include <sstream>

using namespace geode::prelude;

static std::string hsvToString(ccHSVValue const& hsv) {
    return fmt::format("{:.6f}|{:.6f}|{:.6f}", hsv.h, hsv.s, hsv.v);
}

static bool stringToHSV(std::string const& str, ccHSVValue& out) {
    std::stringstream ss(str);
    std::string part;

    if (!std::getline(ss, part, '|')) return false;
    out.h = std::stof(part);

    if (!std::getline(ss, part, '|')) return false;
    out.s = std::stof(part);

    if (!std::getline(ss, part, '|')) return false;
    out.v = std::stof(part);

    return true;
}

class $modify(HSVUtils, ConfigureHSVWidget) {
    bool init(ccHSVValue hsv, bool noBackground, bool addInputs) {
        if (!ConfigureHSVWidget::init(hsv, noBackground, addInputs)) return false;

        auto copyBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_copyBtn_001.png"),
            this,
            menu_selector(HSVUtils::onCopyBtn)
        );
        auto pasteBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_pasteBtn_001.png"),
            this,
            menu_selector(HSVUtils::onPasteBtn)
        );

        auto btnMenu = CCMenu::create();
        btnMenu->setLayout(ColumnLayout::create());
        btnMenu->setPosition(120.f, 25.f);
        btnMenu->setContentSize({40.f, 150.f});
        btnMenu->setScale(0.6f);
        btnMenu->setPosition(119.f, 17.f);

        btnMenu->addChild(pasteBtn, 999);
        btnMenu->addChild(copyBtn, 999);
        btnMenu->updateLayout();
        btnMenu->setTouchEnabled(true);
        btnMenu->setTouchPriority(-600);
        btnMenu->registerWithTouchDispatcher();
        this->addChild(btnMenu);

        return true;
    }

    void onCopyBtn(CCObject*) {
        Mod::get()->setSavedValue("copied-hsv", hsvToString(m_hsv));
        Notification::create("HSV copied.", NotificationIcon::Success)->show();
    }

    void onPasteBtn(CCObject*) {
        auto str = Mod::get()->getSavedValue<std::string>("copied-hsv", "");
        if (str.empty()) {
            Notification::create("No HSV copied.", NotificationIcon::Error)->show();
            return;
        }

        ccHSVValue hsv{};
        if (!stringToHSV(str, hsv)) {
            Notification::create("Invalid HSV data.", NotificationIcon::Error)->show();
            return;
        }

        m_updating = true;

        m_hsv = hsv;
        updateSliders();
        updateLabels();

        m_updating = false;

        Notification::create("HSV pasted.", NotificationIcon::Success)->show();
    }
};
