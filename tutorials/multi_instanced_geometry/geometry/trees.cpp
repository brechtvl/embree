// ======================================================================== //
// Copyright 2009-2019 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

namespace Trees {
  static const float localX[] = {
    -0.312215f, 1.15829f, 0.0298466f, 0.0434759f, 1.19264f, 0.125389f,
    -0.156787f, 1.18601f, -0.093827f, 0.015373f, 1.19541f, -0.103747f,
    0.0150759f, 1.19801f, -0.0674336f, 0.1704f, 1.18709f, 0.0421019f,
    0.0172126f, 1.19686f, -0.0849527f, -0.0651845f, 1.19818f, -0.0111566f,
    0.0640043f, 1.19241f, 0.1186f, 0.00310121f, 1.19381f, 0.121668f, -0.109586f,
    1.19047f, -0.103762f, -0.154614f, 1.18557f, 0.102578f, 0.24576f, 1.17449f,
    0.0132387f, -0.0704072f, 1.19488f, -0.0855343f, 0.0223736f, 1.19347f,
    -0.123f  };
  static const float localY[] = {
    1.05284f, 0.270698f, 0.508174f, -0.592165f, -0.0877563f, 1.04002f,
    0.740146f, 0.171331f, 0.928887f, 1.03221f, -0.0660782f, -0.608425f,
    0.951333f, 0.0291652f, 0.730832f, -1.14488f, 0.175469f, -0.313797f,
    1.11294f, 0.0158262f, 0.448467f, -0.698327f, -0.0289084f, 0.97545f,
    1.19822f, -0.0649494f, 0.00636224f, 0.401221f, -0.115698f, 1.12501f,
    -0.55085f, -0.142792f, -1.05649f, -1.10115f, -0.181756f, 0.440955f,
    0.352402f, -0.0866327f, 1.14381f, 0.970414f, 0.00665169f, -0.70587f,
    0.662059f, -0.114877f, -0.994224f  };
  static const float localZ[] = {
    0.483777f, 0.158403f, -1.08668f, 1.04281f, -0.0995555f, 0.585353f,
    0.931452f, 0.0634932f, -0.753901f, -0.61181f, -0.0814458f, -1.02911f,
    0.731258f, -0.0626418f, -0.949388f, -0.316578f, 0.00439115f, 1.15748f,
    0.448416f, -0.0852221f, -1.1098f, 0.973699f, 0.0594793f, 0.698836f,
    0.0127411f, 0.118086f, -1.19411f, 1.13093f, 0.0377724f, -0.399451f,
    -1.06045f, -0.0488496f, 0.559516f, 0.451188f, -0.0373128f, 1.11132f,
    1.12045f, -0.230366f, -0.362653f, -0.702381f, -0.110585f, -0.96666f,
    -1.00059f, -0.0493243f, -0.660598f  };
  static const float position[] = {
    -65.7305f, 12.1703f, 2.30082f, 25.8744f, 15.4826f, 57.866f, 43.568f,
    14.0236f, 7.55411f, 20.0783f, 12.3845f, 18.2097f, 25.8134f, 13.2488f,
    41.4584f, 7.82571f, 13.4425f, -29.1205f, 2.29001f, 13.032f, -49.5482f,
    28.5152f, 9.39972f, -11.8231f, 23.0445f, 11.2887f, -28.3422f, 31.9229f,
    11.1873f, -33.6449f, 55.575f, 13.8628f, -11.1904f, -56.2014f, 15.7683f,
    -7.40917f, -4.31993f, 16.2341f, -18.621f, -40.7668f, 17.6439f, -26.5077f,
    -48.1934f, 15.8863f, -44.3991f  };
  static const Instances instances = {
    15,
    localX,
    localY,
    localZ,
    position
  };
} // namespace Trees
