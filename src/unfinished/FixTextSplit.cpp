#include "FixTextSplit.hpp"
#include <Geode/utils/base64.hpp>

bool FixTextSplit::onToggled(bool state) {
    return true;
}

void FTSCustomizeObjectLayer::onBreakApart(cocos2d::CCObject* sender) {
    std::vector<TextObjectAttributes> points;

    std::unordered_map<GameObject*, std::unordered_map<int, std::string>> objectAttributes;

    if (m_targetObject) {
        if (m_targetObject->m_objectID != 914) return;
        auto children = m_targetObject->getChildrenExt();
        auto textObj = static_cast<TextGameObject*>(m_targetObject);

        for (int i = 0; i < children.size(); i++) {
            auto world = textObj->convertToWorldSpace(children[i]->getPosition());
            auto node = LevelEditorLayer::get()->m_objectLayer->convertToNodeSpace(world);
            points.push_back({textObj->m_text[i], node.x, textObj});
        }

        for (const auto& [k, v] : asp::iter::split(textObj->getSaveString(GJBaseGameLayer::get()), ',').arrayChunks<2>()) {
            auto num = numFromString<int>(k);
            if (!num) continue;
            objectAttributes[m_targetObject][num.unwrap()] = v;
        }
    }

    if (m_targetObjects) {
        for (auto obj : m_targetObjects->asExt<GameObject*>()) {
            if (obj->m_objectID != 914) continue;
            auto children = obj->getChildrenExt();
            auto textObj = static_cast<TextGameObject*>(obj);

            for (int i = 0; i < children.size(); i++) {
                auto world = textObj->convertToWorldSpace(children[i]->getPosition());
                auto node = LevelEditorLayer::get()->m_objectLayer->convertToNodeSpace(world);
                points.push_back({textObj->m_text[i], node.x, textObj});
            }

            for (const auto& [k, v] : asp::iter::split(textObj->getSaveString(GJBaseGameLayer::get()), ',').arrayChunks<2>()) {
                auto num = numFromString<int>(k);
                if (!num) continue;
                objectAttributes[obj][num.unwrap()] = v;
            }
        }
    }

    StringBuffer<> stringBuffer;

    for (const auto& attr : points) {
        if (std::isspace(attr.c)) continue;
        stringBuffer.append(buildTextObject(attr, objectAttributes[attr.object]));
    }

    if (m_targetObject) {
        LevelEditorLayer::get()->removeObject(m_targetObject, false);
    }

    if (m_targetObjects) {
        for (auto obj : m_targetObjects->asExt<GameObject*>()) {
            LevelEditorLayer::get()->removeObject(obj, false);
        }
    }

    auto editor = EditorUI::get();

    auto objects = editor->pasteObjects(stringBuffer.str(), false, false);

    editor->deselectAll();
    editor->selectObjects(objects, true);
    editor->updateButtons();
    
    onClose(sender);
}

CCPoint getPoint(float x1, float x2, float x, float angleRadians) {
    if (x1 < x2) x = std::max(x1, std::min(x, x2));
    else x = std::max(x2, std::min(x, x1));

    float d = x - x1;

    float cosA = cosf(angleRadians);
    float sinA = sinf(angleRadians);

    return CCPoint(x1 + d * cosA, d * sinA);
}

std::string FTSCustomizeObjectLayer::buildTextObject(const TextObjectAttributes& attributes, const std::unordered_map<int, std::string>& objectAttributes) {
    StringBuffer<> stringBuffer;
    
    bool first = true;

    for (const auto& [k, v] : objectAttributes) {
        if (!first) {
            stringBuffer.append(",");
        }
        first = false;

        auto point = getPoint(attributes.object->boundingBox().getMinX(), attributes.object->boundingBox().getMinX() + attributes.object->getScaledContentWidth(), attributes.x, CC_DEGREES_TO_RADIANS(attributes.object->getRotation()));

        float y = attributes.object->m_positionY - point.y;

        stringBuffer.append("{},", k);
        switch (k) {
            case 2: {
                stringBuffer.append(numToString(attributes.x));
                break;
            }
            case 3: {
                stringBuffer.append(numToString(y));
                break;
            }
            case 31: {
                stringBuffer.append(geode::utils::base64::encode(std::string(1, attributes.c)));
                break;
            }
            default: {
                stringBuffer.append(v);
                break;
            }
        }
    }

    stringBuffer.append(";");

    return stringBuffer.str();
}