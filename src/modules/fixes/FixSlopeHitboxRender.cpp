#include "modules/fixes/FixSlopeHitboxRender.hpp"

bool FixSlopeHitboxRender::onToggled(bool state) {
    return true;
}

bool FSHREditorUI::isSpecialSnapObject(int id) {
    switch (id) {
        case 2896:
        case 2897:
        case 1338:
        case 673:
        case 1341:
        case 1344:
        case 1743:
        case 1745:
        case 1747:
        case 1749:
        case 1906:
        case 294:
        case 326:
        case 349:
        case 289:
        case 709:
        case 711:
        case 726:
        case 728:
        case 321:
        case 331:
        case 343:
        case 353:
        case 337:
        case 483:
        case 492:
        case 651:
        case 886:
        case 299:
        case 309:
        case 315:
            return true;
    }
    return EditorUI::isSpecialSnapObject(id);
}