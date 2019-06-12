import sjson
import json
import pytest

test_case = [
    '{"a": 1}',
    '{"a": 1.1}',
    '{"a": null}',
    '{"a": "string test"}',
    '{"a": true}',
    '{"a": false}',
    '{"a": 1, "b": 2}',
    '{"a": 1, "b": 2, "c": 3}',
    '{"a": 1, "b": 2, "c": 3, "d": 1.1}',
    '{"a": [1, 1.1], "b": 2}',
    '{"a": [1, 1.1], "b": {"nest": "a"} }',
    '{"a": [1, 1.1], "b": {"nest": [1, 3, 5]} }',
    '{"a": [1, 1.1], "b": {"nest": {"d": 1} } }',
    '{"a": [1, 1.1], "b": {"nest": {"d": [1, 3, 2.1]} } }',
]


@pytest.mark.parametrize('json_string', test_case)
def test_json_loads(json_string):
    assert sjson.loads(json_string) == json.loads(json_string)


if __name__ == "__main__":
    pytest.main([__file__])