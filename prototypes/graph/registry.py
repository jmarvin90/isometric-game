from typing import Any, Callable, ClassVar
from dataclasses import dataclass, field

@dataclass(frozen=True)
class Entity:
    _id: ClassVar[int] = 0  # class-level counter (shared)
    id: int = field(init=False)  # instance-level id

    def __post_init__(self):
        # increment the counter and assign id
        cls = type(self)
        object.__setattr__(self, "id", cls._id)
        cls._id += 1

class Registry:
    def __init__(self):
        self.entities = set()
        self.components = {}
        self.callbacks = {}
        self.context = {}

    def create(self) -> Entity:
        entity = Entity()
        self.entities.add(entity)
        return entity
    
    def context_add(self, _type: Any, *args) -> Any:
        instance = _type(*args)
        self.context[_type] = instance
        return instance
    
    def context_get(self, _type: Any) -> Any:
        return self.context[_type]
    
    def add_callback(self, _type: Any, func: Callable) -> None:
        self.callbacks[_type] = func
    
    def destroy(self, id: int) -> None:
        self.entities.remove(id)

    def add_component(self, entity: Entity, _type: Any, *args: Any) -> None:
        if not entity in self.components.keys():
            self.components[entity] = {_type: _type(*args)}
        else:
            self.components[entity][_type] = _type(*args)

        if _type in self.callbacks.keys():
            self.callbacks[_type](self, entity)

    def remove_component(self, entity: Entity, _type: Any) -> None:
        if (
            entity in self.components.keys() and 
            _type in self.components[entity].keys()
        ):
            del self.components[entity][_type]

    def get_component(self, entity: Entity, component_type: Any) -> Any:
        if (
            entity in self.components.keys() and
            component_type in self.components[entity].keys()
        ):
            return self.components[entity][component_type]