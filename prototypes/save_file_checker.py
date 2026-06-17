from __future__ import annotations
import pathlib
import json
import pydantic
import math
import warnings
import typing
import pprint
import typing_extensions
import pandas

LOAD_SAVE_FILE_FROM="save.json"
SAVE_UPDATES_TO="updated_save.json"

def _has_duplicates(value: list) -> bool:
    if (len(value) != (len(set(value)))):
        return True
    return False

class Vec2(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")
    x: int | float
    y: int | float

    def __mul__(self, comparator: Vec2) -> Vec2:
        return Vec2(x=self.x * comparator.x, y=self.y * comparator.y)

class BuildingPairComponent(pydantic.BaseModel):
    paired_with: int

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
        if (_has_duplicates(value)):
            warnings.warn("Duplicates in container")
        return value

    def clear(self) -> None:
        self.entities = []
        self.segments = []
        print("Cleared!")

class CellSpanComponent(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")
    AA: Vec2
    BB: Vec2

class Grid(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")
    cell_size: Vec2
    cells: list[int]
    grid_dimensions: Vec2

    @pydantic.computed_field
    @property
    def grid_area(self) -> Vec2:
        return self.grid_dimensions * self.cell_size
        
    @pydantic.field_validator("cells")
    @classmethod
    def square_warning(cls, value: list[int]) -> list[int]:
        root = math.sqrt(len(value))
        if (len(value) != root ** 2):
            raise ValueError("Spatial map structure isn't perfect square")
        return value

    @pydantic.field_validator("cells")
    @classmethod
    def duplicate_check(cls, value: list[int]) -> list[int]:
        if (_has_duplicates(value)):
            warnings.warn("Duplicates in container")
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
    @pydantic.model_validator(mode="after")
    def spatial_and_tilemap_align(self) -> typing_extensions.Self:
        if (self.spatialmap.grid_area != self.tilemap.grid_area):
            raise ValueError("The tilemap and the spatial partition have different total areas")
        return self

ComponentType = typing.Annotated[
    GridPositionComponent 
    | TransformComponent 
    | SpriteComponent 
    | SpatialMapCellComponent 
    | CellSpanComponent
    | BuildingPairComponent,
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
        if (_has_duplicates(value)):
            warnings.warn("Duplicates in container")
        return value


class SaveFile(pydantic.BaseModel):
    component_pools: list[ComponentPool]
    context: Context

    def as_pandas(self) -> pandas.DataFrame:
        output = pandas.DataFrame()
        for component_pool in self.component_pools:
            if component_pool.components:
                output[str(type(component_pool.components[0]))] = \
                    pandas.Series(True, index=component_pool.entities)
        return output


save_file = pathlib.Path(LOAD_SAVE_FILE_FROM)
data = json.loads(save_file.open("r").read())

try:
    save_file = SaveFile.model_validate(data)
    for pool in save_file.component_pools:
        if (not pool.components):
            continue
        print(type(pool.components[0]))
        if isinstance(pool.components[0], SpatialMapCellComponent):
            print("clearing...")
            for component in pool.components:
                component.clear()
    updated_save_file = pathlib.Path(SAVE_UPDATES_TO).open("w").write(
        json.dumps(
            save_file.model_dump(exclude_computed_fields=True)
        )
    )
    dataframe = save_file.as_pandas()
    pathlib.Path("tabular_output.txt").open("w").write(dataframe.to_string())
except pydantic.ValidationError as e:
    pprint.pprint(e.errors())