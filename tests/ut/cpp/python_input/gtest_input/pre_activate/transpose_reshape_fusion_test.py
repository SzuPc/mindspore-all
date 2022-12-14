# Copyright 2020-2022 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================
from mindspore.ops import Primitive
from mindspore.ops import operations as P

Transpose = P.Transpose()
Reshape = P.Reshape()
BatchMatMul = P.BatchMatMul()
ConfusionTransposeD = Primitive('ConfusionTransposeD')
make_tuple = Primitive('MakeTuple')


class FnDict:
    def __init__(self):
        self.fnDict = {}

    def __call__(self, fn):
        self.fnDict[fn.__name__] = fn

    def __getitem__(self, name):
        return self.fnDict[name]


def test_transpose_reshape_fusion(tag):
    """test transpose_reshape_fusion net."""
    fns = FnDict()

    @fns
    def before(x):
        transpose = Transpose(x, (1, 0, 2, 3))
        reshape = Reshape(transpose, (2, 2, 16, 16))
        res = BatchMatMul(reshape, reshape)
        return res

    @fns
    def after(x):
        confusion = ConfusionTransposeD(x)
        res = make_tuple(BatchMatMul(confusion, confusion))
        return res

    return fns[tag]
