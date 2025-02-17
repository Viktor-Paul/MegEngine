# MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
#
# Copyright (c) 2014-2021 Megvii Inc. All rights reserved.
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

from . import compat
from ._passes import optimize
from .pytree import register_supported_type
from .tm_config import disable_default_checker, enable_expr_checker
from .traced_module import (
    TracedModule,
    _register_all_builtin_module,
    register_as_builtin,
    trace_module,
    wrap,
)

_register_all_builtin_module()

__all__ = [
    "register_as_builtin",
    "register_supported_type",
    "trace_module",
    "wrap",
    "TracedModule",
    "optimize",
    "enable_expr_checker",
    "disable_default_checker",
]
