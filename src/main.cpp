/**,
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/async.hpp> 
#include <Geode/loader/Event.hpp>

/**
 * Brings cocos2d and all Geode namespaces to the current scope.
 */
using namespace geode::prelude;

class $modify(LevelInfoLayer) {
	struct Fields {
		async::TaskHolder<web::WebResponse> m_listListener{};
	};

	void drawDemon(std::string_view resource) {
		auto demonSprite{ CCSprite::create(resource.data()) };
		demonSprite->setPosition({ 184.5, 231.0 });
		this->addChild(demonSprite);
	}


	bool init(GJGameLevel* level, bool challenge) {
		if (!LevelInfoLayer::init(level, challenge)) {
			return false;
		}


		auto request = web::WebRequest();
		m_fields->m_listListener.spawn(
			request.get(fmt::format("https://api.aredl.net/v2/api/aredl/levels/{}", level->m_levelID)),
			[this](web::WebResponse result) {
				log::debug("response {}", result.string().unwrap());
				std::string rawJson{ result.string().unwrap() };
				auto parsedJson{ matjson::parse(rawJson).unwrapOrDefault() };


				if (parsedJson.contains("position")) {
					geode::log::debug("level \"{}\", position {}", parsedJson["name"].asString().unwrap(), parsedJson["position"].asInt().unwrap());
					intmax_t position{ parsedJson["position"].asInt().unwrap() };
					this->getChildByID("difficulty-sprite")->setVisible(false);
						
					if (position <= 150 && position > 75) {
						drawDemon("cardemon.png"_spr);
					}
					else if (position <= 75) {
						drawDemon("uwudemon.png"_spr);
					}

				} else {
					log::debug("not extreme demon");
				}
			}
	    );


		log::debug("{}", level->m_levelID);

		return true;
	}
};


