#pragma once

using namespace geode::prelude;

namespace ObjectIDDisplay {
	class AddObjectIDLabelEvent final : public geode::Event<AddObjectIDLabelEvent, bool(CreateMenuItem*)> {
	public:
		CreateMenuItem* buttonWithGameObject {};
		inline geode::Result<> addObjectIDLabel(CreateMenuItem* button) {
			if (!button) {
				log::error("[ObjectIDDisplay API] BUTTON WAS NULLPTR");
				return Err(fmt::format("[ObjectIDDisplay API] BUTTON WAS NULLPTR"));
			}
			AddObjectIDLabelEvent().send(button);
			return Ok();
		}
	};
}