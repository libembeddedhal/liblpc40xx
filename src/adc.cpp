// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <libhal-lpc40/adc.hpp>

#include <libhal-lpc40/clock.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/pin.hpp>
#include <libhal-lpc40/power.hpp>
#include <libhal-util/bit.hpp>
#include <libhal-util/bit_limits.hpp>

#include "adc_reg.hpp"

namespace hal::lpc40 {

status setup(const adc::channel& p_channel)
{
  using namespace hal::literals;

  if (p_channel.clock_rate > 1.0_MHz) {
    return hal::new_error();
  }

  if (p_channel.index >= adc_reg_t::channel_length) {
    return hal::new_error();
  }

  power(peripheral::adc).on();

  // For proper operation, analog pins must be set to floating.
  p_channel.adc_pin.function(p_channel.pin_function)
    .resistor(hal::pin_resistor::none)
    .open_drain(false)
    .analog(true);

  const auto clock_frequency = clock::get().get_frequency(peripheral::adc);
  const auto clock_divider = clock_frequency / p_channel.clock_rate;
  const auto clock_divider_int = static_cast<std::uint32_t>(clock_divider);

  // Activate burst mode (continuous sampling), power on ADC and set clock
  // divider.
  hal::bit::modify(adc_reg->control)
    .set<adc_control_register::burst_enable>()
    .set<adc_control_register::power_enable>()
    .insert<adc_control_register::clock_divider>(clock_divider_int);

  // Enable channel. Must be done in a separate write to memory than power on
  // and burst enable.
  hal::bit::modify(adc_reg->control)
    .set(bit::mask{ .position = p_channel.index, .width = 1 });

  return hal::success();
}

static constexpr adc::channel get_channel_info(std::uint8_t p_channel)
{
  enum adc_function : uint8_t
  {
    pin_0123 = 0b001,
    pin_4567 = 0b011
  };
  constexpr std::array channels{
    adc::channel{
      .adc_pin = pin(0, 23),
      .index = 0,
      .pin_function = adc_function::pin_0123,
    },
    adc::channel{
      .adc_pin = pin(0, 24),
      .index = 1,
      .pin_function = adc_function::pin_0123,
    },
    adc::channel{
      .adc_pin = pin(0, 25),
      .index = 2,
      .pin_function = adc_function::pin_0123,
    },
    adc::channel{
      .adc_pin = pin(0, 26),
      .index = 3,
      .pin_function = adc_function::pin_0123,
    },
    adc::channel{
      .adc_pin = pin(1, 30),
      .index = 4,
      .pin_function = adc_function::pin_4567,
    },
    adc::channel{
      .adc_pin = pin(1, 31),
      .index = 5,
      .pin_function = adc_function::pin_4567,
    },
    adc::channel{
      .adc_pin = pin(0, 12),
      .index = 6,
      .pin_function = adc_function::pin_4567,
    },
    adc::channel{
      .adc_pin = pin(0, 13),
      .index = 7,
      .pin_function = adc_function::pin_4567,
    },
  };

  return channels[p_channel];
}

result<adc> adc::get(size_t p_channel)
{
  auto channel_info = get_channel_info(p_channel);
  HAL_CHECK(setup(channel_info));
  adc adc_channel(channel_info);
  return adc_channel;
}

/**
 * @brief Construct a custom adc object based on the passed in channel
 * information.
 *
 * Care should be taken to ensure that the adc's operating frequency does not
 * go above 1MHz and that the the channel index is within the bounds of 0
 * to 7. Exceeding these bounds will result in a call to std::abort.
 *
 * Care should also be taken to ensure that two adc's constructed via this
 * method do not overlap in index.
 *
 * The operating frequency is shared across all adc channels, which means that
 * the last adc to be constructed will set sampling frequency for all
 * channels.
 *
 * @param p_channel - Which adc channel to return
 * @return adc& - statically allocated adc object
 */
result<adc> adc::construct_custom_channel(const channel& p_channel)
{
  HAL_CHECK(setup(p_channel));
  adc adc_channel(p_channel);
  return adc_channel;
}

adc::adc(const channel& p_channel)
  : m_sample(&adc_reg->data[p_channel.index])
{
}

result<adc::read_t> adc::driver_read()
{
  constexpr auto max = bit_limits<12, size_t>::max();
  constexpr auto max_float = static_cast<float>(max);
  // Read sample from peripheral memory
  auto sample_integer = hal::bit::extract<adc_data_register::result>(*m_sample);
  auto sample = static_cast<float>(sample_integer);
  return read_t{ .sample = sample / max_float };
}

}  // namespace hal::lpc40