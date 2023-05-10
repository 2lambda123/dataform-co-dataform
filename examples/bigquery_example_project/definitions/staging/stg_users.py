view(
    {
        "schema": "staging",
        "description": "Cleaned version of stackoverflow.users table",
        "tags": ["staging"],
    }
).sql(
    f"""
select
    id as user_id,
    age,
    creation_date,
    round(timestamp_diff(current_timestamp(), creation_date, day)/365) as user_tenure
from
    {ref("users")}
"""
)
