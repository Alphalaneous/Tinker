#include "modules/MoreAttributes.hpp"

bool MoreAttributes::onToggled(bool state) {
    return true;
}

bool MASetupKeyframePopup::init(KeyframeGameObject* object, cocos2d::CCArray* objects, LevelEditorLayer* layer) {
    if (!SetupKeyframePopup::init(object, objects, layer)) return false;

    auto winSize = CCDirector::get()->getWinSize();

    auto point = CCPoint{winSize.width / 2.f - 110.f, winSize.height / 2.f - 90.f};
    auto arr = createValueControlAdvanced(373, "KeyframeID", point, 0.7f, true, InputValueType::Uint, 6, true, 0, 100, 1, 0, GJInputStyle::GoldLabel, 2, false);

    bool shared = true;
    int keyframeID = 0;
    if (!object) {
        keyframeID = objects->asExt<KeyframeGameObject>()[0]->m_keyframeGroup;
        for (auto obj : objects->asExt<KeyframeGameObject>()) {
            if (keyframeID != obj->m_keyframeGroup) {
                shared = false;
                break;
            }
        }
    }
    else {
        keyframeID = static_cast<KeyframeGameObject*>(object)->m_keyframeGroup;
    }

    if (shared) {
        for (auto node : arr->asExt<CCNode>()) {
            if (auto input = typeinfo_cast<CCTextInputNode*>(node)) {
                input->setString(numToString(keyframeID));
                break;
            }
        }
    }

    return true;
}

void MASetupKeyframePopup::valueDidChange(int tag, float value) {
    SetupKeyframePopup::valueDidChange(tag, value);
    if (tag == 373) {
        CCArray* objs;
        if (!m_gameObject) {
            objs = m_gameObjects;
        }
        else {
            objs = CCArray::createWithObject(m_gameObject);
        }

        CCArray* newArr;
        for (auto obj : objs->asExt<KeyframeGameObject>()) {
            auto arr = static_cast<CCArray*>(m_editorLayer->m_keyframeGroups->objectForKey(obj->m_keyframeGroup));
            if (arr) {
                arr->removeObject(obj);
                if (arr->count() == 0) {
                    m_editorLayer->m_keyframeGroups->removeObjectForKey(obj->m_keyframeGroup);
                }
            }

            obj->m_keyframeGroup = value;

            newArr = static_cast<CCArray*>(m_editorLayer->m_keyframeGroups->objectForKey(value));
            if (!newArr) {
                newArr = CCArray::create();
                m_editorLayer->m_keyframeGroups->setObject(newArr, value);
            }
            newArr->addObject(obj);

            m_editorLayer->updateKeyframeObjects();
        }

        if (newArr) {
            auto asExt = CCArrayExt<KeyframeGameObject>(newArr);
            std::sort(
                asExt.begin(),
                asExt.end(),
                [](KeyframeGameObject* a, KeyframeGameObject* b) {
                    return a->m_keyframeIndex < b->m_keyframeIndex;
                }
            );
        }
    }
}

bool MASetupAdvFollowPopup::init(AdvancedFollowTriggerObject* object, cocos2d::CCArray* objects) {
    if (!SetupAdvFollowPopup::init(object, objects)) return false;

    bool ignoreGroupParent = false;
    bool ignoreLinked = false;
    if (!object) {
        auto first = objects->asExt<AdvancedFollowTriggerObject>()[0];
        ignoreGroupParent = first->m_ignoreGroupParent;
        ignoreLinked = first->m_ignoreLinkedObjects;
        for (auto obj : objects->asExt<KeyframeGameObject>()) {
            if (ignoreGroupParent != obj->m_ignoreGroupParent) {
                ignoreGroupParent = false;
            }
            if (ignoreLinked != obj->m_ignoreLinkedObjects) {
                ignoreLinked = false;
            }
        }
    }
    else {
        ignoreGroupParent = static_cast<AdvancedFollowTriggerObject*>(object)->m_ignoreGroupParent;
        ignoreLinked = static_cast<AdvancedFollowTriggerObject*>(object)->m_ignoreLinkedObjects;
    }

    auto winSize = CCDirector::get()->getWinSize();
    auto point = CCPoint{winSize.width / 2.f + m_width / 2.f - 170.f, winSize.height / 2.f - m_height / 2.f + 20.f};

    createCustomToggleValueControl(280, ignoreGroupParent, false, "Ignore\nGParent", point, false, 0, 0);
    createCustomToggleValueControl(281, ignoreLinked, false, "Ignore\nLinked", point + CCPoint{100.f, 0.f}, false, 0, 0);

    return true;
}

void MASetupAdvFollowPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender) {
    auto tag = sender->getTag();

    if (tag == 280 || tag == 281) {
        auto toggler = static_cast<CCMenuItemToggler*>(sender);
        CCArray* objs;
        if (!m_gameObject) {
            objs = m_gameObjects;
        }
        else {
            objs = CCArray::createWithObject(m_gameObject);
        }

        for (auto obj : objs->asExt<AdvancedFollowTriggerObject>()) {
            if (tag == 280) {
                obj->m_ignoreGroupParent = !toggler->isToggled();
            }
            if (tag == 281) {
                obj->m_ignoreLinkedObjects = !toggler->isToggled();
            }
        }
        
        toggler->toggle(!toggler->isToggled());
        return;
    }

    SetupAdvFollowPopup::onCustomToggleTriggerValue(sender);
}

bool MASetupInstantCollisionTriggerPopup::init(EffectGameObject* object, cocos2d::CCArray* objects) {
    if (!SetupInstantCollisionTriggerPopup::init(object, objects)) return false;

    bool remapFix = false;
    if (!object) {
        remapFix = objects->asExt<EffectGameObject>()[0]->m_isPost2208Remap;
        for (auto obj : objects->asExt<KeyframeGameObject>()) {
            if (remapFix != obj->m_isPost2208Remap) {
                remapFix = false;
                break;
            }
        }
    }
    else {
        remapFix = static_cast<EffectGameObject*>(object)->m_isPost2208Remap;
    }

    auto winSize = CCDirector::get()->getWinSize();
    auto point = CCPoint{winSize.width / 2.f + m_width / 2.f - 72.f, winSize.height / 2.f - m_height / 2.f + 50.f};

    createToggleButton("Remap\nFix", menu_selector(MASetupInstantCollisionTriggerPopup::onRemapFix), remapFix, point);

    return true;
}

void MASetupInstantCollisionTriggerPopup::onRemapFix(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    CCArray* objs;
    if (!m_gameObject) {
        objs = m_gameObjects;
    }
    else {
        objs = CCArray::createWithObject(m_gameObject);
    }

    for (auto obj : objs->asExt<AdvancedFollowTriggerObject>()) {
        obj->m_isPost2208Remap = !toggler->isToggled();
    }
}

bool MASetupObjectOptions2Popup::init(GameObject* object, cocos2d::CCArray* objects) {
    if (!SetupObjectOptions2Popup::init(object, objects)) return false;

    auto winSize = CCDirector::get()->getWinSize();
    auto point = CCPoint{winSize.width / 2.f - 80.f, winSize.height / 2.f - 25.f};

    auto arr = createValueControlAdvanced(108, "LinkedID:", point, 0.7f, true, InputValueType::Uint, 6, true, 0, 100, 0, 0, GJInputStyle::GoldLabel, 2, false);

    bool shared = true;
    int linkedID = 0;
    if (!object) {
        linkedID = objects->asExt<GameObject>()[0]->m_linkedGroup;
        for (auto obj : objects->asExt<GameObject>()) {
            if (linkedID != obj->m_linkedGroup) {
                shared = false;
                break;
            }
        }
    }
    else {
        linkedID = static_cast<GameObject*>(object)->m_linkedGroup;
    }

    for (auto node : arr->asExt<CCNode>()) {
        if (auto input = typeinfo_cast<CCTextInputNode*>(node)) {
            if (shared) {
                input->setString(numToString(linkedID));
            }

            auto oldStr = input->getString();

            addOnExitCallback([this, input, oldStr] {
                if (input->getString() == oldStr) return;

                CCArray* objs;
                if (!m_gameObject) {
                    objs = m_gameObjects;
                }
                else {
                    objs = CCArray::createWithObject(m_gameObject);
                }
                auto numRes = numFromString<int>(input->getString());

                for (auto obj : objs->asExt<AdvancedFollowTriggerObject>()) {
                    if (numRes) {
                        auto dict = LevelEditorLayer::get()->m_linkedGroupDict;

                        auto oldArr = static_cast<CCArray*>(dict->objectForKey(obj->m_linkedGroup));
                        if (oldArr) {
                            oldArr->removeObject(obj);
                            if (oldArr->count() == 0) {
                                dict->removeObjectForKey(obj->m_linkedGroup);
                            }
                        }

                        obj->m_linkedGroup = numRes.unwrap();

                        auto newArr = static_cast<CCArray*>(dict->objectForKey(obj->m_linkedGroup));
                        if (!newArr) {
                            newArr = CCArray::create();
                            dict->setObject(newArr, obj->m_linkedGroup);
                        }
                        newArr->addObject(obj);
                    }
                }
            });
            break;
        }
    }

    return true;
}

void MASetupTriggerPopup::createMultiTriggerItems(cocos2d::CCPoint touchPos, cocos2d::CCPoint spawnPos, cocos2d::CCPoint multiPos) {
    if (typeinfo_cast<SetupAdvFollowPopup*>(this)) {
        auto winSize = CCDirector::get()->getWinSize();
        multiPos = CCPoint{winSize.width / 2.f - 200.f, winSize.height / 2.f - 110.f};
    }
    SetupTriggerPopup::createMultiTriggerItems(touchPos, spawnPos, multiPos);
}

cocos2d::CCArray* MASetupTriggerPopup::createToggleValueControl(int property, gd::string label, cocos2d::CCPoint position, bool vertical, int page, int group, float scale) {
    if (typeinfo_cast<SetupAdvFollowPopup*>(this)) {
        if (property == 571) {
            position.y += 8.5f;
        }
    }
    return SetupTriggerPopup::createToggleValueControl(property, label, position, vertical, page, group, scale);
}

cocos2d::CCArray* MASetupTriggerPopup::createCustomToggleValueControl(int property, bool toggled, bool notClickable, gd::string text, cocos2d::CCPoint position, bool vertical, int page, int group) {
    if (typeinfo_cast<SetupAdvFollowPopup*>(this)) {
        if (property == 0 || property == 1 || property == 2) {
            position.y += 8.5f;
        }
    }
    return SetupTriggerPopup::createCustomToggleValueControl(property, toggled, notClickable, text, position, vertical, page, group);
}