#include <components/building_pair_component.h>
#include <components/flags.h>
#include <components/sprite_component.h>
#include <entt/entt.hpp>
#include <sprite.h>
#include <systems/building_system.h>

namespace {

void unpair(entt::registry& registry, entt::entity entity)
{
    if (!registry.all_of<BuildingPairComponent>(entity))
        return;

    const BuildingPairComponent pair {
        registry.get<const BuildingPairComponent>(entity)
    };

    for (auto paired_entity : { entity, pair.paired_with }) {
        registry.remove<BuildingPairComponent>(paired_entity);
    }
}
}

namespace BuildingSystem {

void untag(entt::registry& registry, entt::entity entity)
{
    unpair(registry, entity);
    registry.remove<SenderFlag, ReceiverFlag>(entity);
}

void tag(entt::registry& registry, entt::entity entity)
{
    untag(registry, entity);

    const SpriteComponent& sprite {
        registry.get<const SpriteComponent>(entity)
    };

    switch (sprite.sprite_definition->sprite_type) {
    case SpriteType::BUILDING_SENDER:
        registry.emplace<SenderFlag>(entity);
        break;
    case SpriteType::BUILDING_RECEIVER:
        registry.emplace<ReceiverFlag>(entity);
        break;
    default:
        break;
    }
}

void update(entt::registry& registry)
{
    auto unpaired_senders {
        registry.view<SenderFlag>(entt::exclude<BuildingPairComponent>)
    };

    auto unpaired_receivers {
        registry.view<ReceiverFlag>(entt::exclude<BuildingPairComponent>)
    };

    auto sendersIt { unpaired_senders.begin() };
    auto receiversIt { unpaired_receivers.begin() };

    while (
        sendersIt != unpaired_senders.end()
        && receiversIt != unpaired_receivers.end()
    ) {
        entt::entity sender { *sendersIt++ };
        entt::entity receiver { *receiversIt++ };
        registry.emplace<BuildingPairComponent>(sender, receiver);
        registry.emplace<BuildingPairComponent>(receiver, sender);
    }
}

}