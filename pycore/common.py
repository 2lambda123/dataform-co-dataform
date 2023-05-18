from dataclasses import dataclass
from typing import Literal, Optional, List, Dict
from protos.core_pb2 import Target, Table as TableProto, TableType, ProjectConfig
import re


@dataclass
class RecordDescriptor:
    """
    A data class that describes a struct, object or record in a dataset that has nested columns.

    Args:
        description: A description of the struct, object or record.
        columns: A description of columns within the struct, object or record.
        displayName: A human-readable name for the column.
        dimension: The type of the column. Can be `category`, `timestamp` or `number`.
        aggregator: The type of aggregator to use for the column. Can be `sum`, `distinct` or `derived`.
        expression: The expression to use for the column.
        tags: Tags that apply to this column (experimental).
        bigqueryPolicyTags: BigQuery policy tags that should be applied to this column.
    """

    description: Optional[str] = None
    columns: Optional[Dict[str, "RecordDescriptor"]] = None


@dataclass
class DocumentableActionConfig:
    """
    A data class that

    Args:
        description:  A description of the dataset.
        columns: A description of columns within the struct, object or record.
    """

    description: Optional[str] = None
    columns: Optional[Dict[str, "RecordDescriptor"]] = None


@dataclass
class TargetableActionConfig:
    """
    A dataclass that with attributes that represent the warehouse target of an action.

    Args:
        database: The database in which the output of this action should be created.
        schema: The schema in which the output of this action should be created.
        name: The name of the action.
    """

    database: Optional[str] = None
    schema: Optional[str] = None
    name: Optional[str] = None


@dataclass
class ActionWithDependenciesConfig:
    """
    A dataclass that represents the dependency attributes of an action.

    Args:
        tags: A list of user-defined tags with which the action should be labeled.
        dependencies: Dependencies of the action.
        disabled: If set to true, this action will not be executed. However, the action may still be depended upon. Useful for temporarily turning off broken actions.
    """

    tags: Optional[List[str]] = None
    dependencies: Optional[List[str]] = None
    disabled: Optional[bool] = None


def action_target(
    project_config: ProjectConfig,
    name: str,
    database_override: Optional[str] = None,
    schema_override: Optional[str] = None,
    name_override: Optional[str] = None,
):
    "Creates the target for an action, processing any overrides where supplied."
    target = Target()
    target.database = (
        database_override if database_override else project_config.default_database
    )
    target.schema = (
        schema_override if schema_override else project_config.default_schema
    )
    target.name = name_override if name_override else name
    return target


def target_to_target_representation(target: Target) -> str:
    """
    Converts a Target (proto) to a target representation (string), e.g. `database.schema.name`.
    """
    return f"{target.database}.{target.schema}.{target.name}"


def efficient_replace_string(to_replace: Dict[str, str], text: str):
    """
    Note: This may only be more efficient for large `to_replace` dicts and long `text` strings.
    """
    # First make the replacements regex safe.
    to_replace = dict((re.escape(k), re.escape(v)) for k, v in to_replace.items())
    # Then apply the "or" matching operator.
    pattern = re.compile("|".join(to_replace.keys()))
    # Finally apply the regex substitution.
    return pattern.sub(lambda match: to_replace[re.escape(match.group(0))], text)
