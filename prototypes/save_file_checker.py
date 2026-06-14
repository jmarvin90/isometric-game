import pathlib
import json
import pydantic
import math
import warnings
import typing
import pprint
import typing_extensions

LOAD_SAVE_FILE_FROM="save.json"
SAVE_UPDATES_TO="updated_save.json"

def duplicate_check(value: list) -> list:
    if (len(value) != (len(set(value)))):
        warnings.warn("Duplicates in container")
    return list(set(value))

class Vec2(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")
    x: int | float
    y: int | float

class GridPositionComponent(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")
    position: Vec2

class TransformComponent(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")
    position: Vec2
    rotation: float
    z_index: int

class SpriteComponent(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")
    name: str

class SpatialMapCellComponent(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")
    entities: list[int]
    segments: list[int]

    @pydantic.field_validator("entities", "segments")
    @classmethod
    def duplicate_check(cls, value: list[int]) -> list[int]:
        return duplicate_check(value)

class CellSpanComponent(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")
    AA: Vec2
    BB: Vec2

class Grid(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")
    cell_size: Vec2
    cells: list[int]
    grid_dimensions: Vec2

    @pydantic.field_validator("cells")
    @classmethod
    def square_warning(cls, value: list[int]) -> list[int]:
        root = math.sqrt(len(value))
        if (len(value) != root ** 2):
            warnings.warn("Spatial map structure isn't perfect square")
        return value

    @pydantic.model_validator(mode="after")
    def check_grid_dimensions(self) -> typing_extensions.Self:
        if (
            len(self.cells) != (
                self.grid_dimensions.x * self.grid_dimensions.y
            )
        ):
            raise ValueError("Contained elements don't match grid dimensions")
        return self


class SpatialMap(Grid):
    pass

class TileMap(Grid):
    pass

class Context(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")
    spatialmap: SpatialMap
    tilemap: TileMap

ComponentType = typing.Annotated[
    GridPositionComponent 
    | TransformComponent 
    | SpriteComponent 
    | SpatialMapCellComponent 
    | CellSpanComponent, 
    pydantic.Field(union_mode="left_to_right") 
]

class ComponentPool(pydantic.BaseModel):
    components: list[ComponentType]
    entities: list[int]
    size: int

    # TODO: list of entities == size

    @pydantic.field_validator("entities")
    @classmethod
    def duplicate_check(cls, value: list[int]) -> list[int]:
        return duplicate_check(value)


class SaveFile(pydantic.BaseModel):
    component_pools: list[ComponentPool]
    context: Context


save_file = pathlib.Path(LOAD_SAVE_FILE_FROM)
data = json.loads(save_file.open("r").read())
try:
    save_file = SaveFile.model_validate(data)
    updated_save_file = pathlib.Path(SAVE_UPDATES_TO).open("w").write(json.dumps(save_file.model_dump()))
    print("Squanch")
except pydantic.ValidationError as e:
    pprint.pprint(e.errors())