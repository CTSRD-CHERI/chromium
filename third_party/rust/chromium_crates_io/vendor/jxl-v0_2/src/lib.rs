// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#![feature(cfg_version)]
#![cfg_attr(not(version("1.77")), feature(array_methods))]
#![cfg_attr(not(version("1.79")), feature(inline_const))]
#![cfg_attr(not(version("1.73")), feature(int_roundings))]
#![cfg_attr(not(version("1.81")), feature(io_slice_advance))]
#![cfg_attr(not(version("1.82")), feature(is_sorted))]
#![cfg_attr(not(version("1.88")), feature(let_chains))]
#![cfg_attr(not(version("1.81")), feature(lint_reasons))]

#![deny(unsafe_code)]
pub mod api;
pub mod bit_reader;
pub mod color;
pub mod container;
pub mod entropy_coding;
pub mod error;
pub mod features;
pub mod frame;
pub mod headers;
pub mod icc;
pub mod image;
pub mod render;
pub mod util;

// TODO: Move these to a more appropriate location.
const GROUP_DIM: usize = 256;
const BLOCK_DIM: usize = 8;
const BLOCK_SIZE: usize = BLOCK_DIM * BLOCK_DIM;
#[allow(clippy::excessive_precision)]
const MIN_SIGMA: f32 = -3.90524291751269967465540850526868;
