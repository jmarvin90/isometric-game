#include <components/building_pair_component.h>
#include <components/flags.h>
#include <components/sprite_component.h>
#include <entt/entt.hpp>
#include <sprite.h>
#include <systems/building_system.h>

namespace {
void unpair(entt::registry& registry, entt::entity entity)
{
    auto pairs { registry.view<BuildingPairComponent>() };
    std::vector<entt::entity> pending_delete {};

    for (auto [pair_entity, pair] : pairs.each()) {
        if (entity != pair.sender && entity != pair.receiver)
            continue;

        pending_delete.emplace_back(pair_entity);

        for (auto pair_member : { pair.sender, pair.receiver }) {
            registry.remove<PairedFlag>(pair_member);
            registry.emplace_or_replace<UnpairedFlag>(pair_member);
        }
    }

    for (auto element : pending_delete) {
        registry.destroy(element);
    }
}
}

namespace BuildingSystem {

void tag(entt::registry& registry, entt::entity entity)
{
    unpair(registry, entity);
    registry.remove<SenderFlag, ReceiverFlag>(entity);

    const SpriteComponent& sprite {
        registry.get<const SpriteComponent>(entity)
    };

    switch (sprite.sprite_definition->sprite_type) {
    case SpriteType::BUILDING_SENDER:
        registry.emplace<SenderFlag>(entity);
        registry.emplace_or_replace<UnpairedFlag>(entity);
        break;
    case SpriteType::BUILDING_RECEIVER:
        registry.emplace<ReceiverFlag>(entity);
        registry.emplace_or_replace<UnpairedFlag>(entity);
        break;
    default:
        break;
    }
}

void untag(entt::registry& registry, entt::entity entity)
{
    unpair(registry, entity);
    registry.remove<SenderFlag, ReceiverFlag>(entity);
}

void update(entt::registry& registry)
{
    auto unpaired_senders { registry.view<UnpairedFlag, SenderFlag>() };
    auto unpaired_receivers { registry.view<UnpairedFlag, ReceiverFlag>() };

    auto sendersIt { unpaired_senders.begin() };
    auto receiversIt { unpaired_receivers.begin() };
    std::vector<entt::entity> matched {};

    while (true) {
        if (
            sendersIt == unpaired_senders.end()
            || receiversIt == unpaired_receivers.end()
        )
            break;

        entt::entity sender { *sendersIt++ };
        entt::entity receiver { *receiversIt++ };
        entt::entity pair { registry.create() };
        registry.emplace<BuildingPairComponent>(pair, sender, receiver);

        for (auto entity : { sender, receiver }) {
            registry.emplace<PairedFlag>(entity);
            matched.emplace_back(entity);
        }
    }

    for (auto entity : matched) {
        registry.remove<UnpairedFlag>(entity);
    }
}

}