/* Copyright (c) 2016 Baidu, Inc. All Rights Reserve.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once
#include <memory>

#include "paddle/gserver/evaluators/Evaluator.h"
#include "Layer.h"

P_DECLARE_int32(trainer_id);

namespace paddle {

class ValidationLayer : public Layer {
public:
  explicit ValidationLayer(const LayerConfig& config) : Layer(config) {}

  bool init(const LayerMap& layerMap, const ParameterMap& parameterMap);

  LayerPtr getOutputLayer() { return inputLayers_[0]; }

  LayerPtr getLabelLayer() { return inputLayers_[1]; }

  LayerPtr getInfoLayer() {
    assert(inputLayers_.size() > 2);
    return inputLayers_[2];
  }

  virtual void forward(PassType passType);

  virtual void backward(const UpdateCallback& callback = nullptr);

  virtual void validationImp(MatrixPtr outputValue, IVectorPtr label) = 0;

  virtual void onPassEnd() = 0;
};

/*
 * AucValidation
 */
class AucValidation : public ValidationLayer {
public:
  explicit AucValidation(const LayerConfig& config)
      : ValidationLayer(config),
        cpuOutput_(nullptr),
        cpuLabel_(nullptr),
        cpuWeight_(nullptr) {}

  bool init(const LayerMap& layerMap, const ParameterMap& parameterMap);

  void validationImp(MatrixPtr outputValue, IVectorPtr label);

  void onPassEnd();

  struct PredictionResult {
    PredictionResult(real __out, int __label) : out(__out), label(__label) {}
    real out;
    int label;
  };
  std::vector<PredictionResult> predictArray_;

private:
  bool passBegin_;
  std::unique_ptr<Evaluator> evaluator_;
  MatrixPtr cpuOutput_;
  IVectorPtr cpuLabel_;
  MatrixPtr cpuWeight_;
};

/*
 * positive-negative pair rate Validation
 */
class PnpairValidation : public ValidationLayer {
public:
  explicit PnpairValidation(const LayerConfig& config)
      : ValidationLayer(config) {}

  bool init(const LayerMap& layerMap, const ParameterMap& parameterMap);

  void validationImp(MatrixPtr outputValue, IVectorPtr label);

  void onPassEnd();

private:
  bool passBegin_;
  std::unique_ptr<Evaluator> evaluator_;
};

typedef std::shared_ptr<ValidationLayer> ValidationLayerPtr;
}  // namespace paddle
