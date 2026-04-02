//! Modbus data model with configurable limits.
//!
//! Implements the complete Modbus data model storing all four data types:
//! - **Coils** — read/write discrete outputs (`bool`)
//! - **Discrete Inputs** — read-only discrete inputs (`bool`)
//! - **Holding Registers** — read/write 16-bit registers (`u16`)
//! - **Input Registers** — read-only 16-bit registers (`u16`)
//!
//! All addresses are 0-based internally. Bounds checking is performed on every
//! access, returning `None` or silently ignoring out-of-range writes.
//! If your application uses Modbus reference notation such as `00001` or
//! `40001`, convert those values to zero-based offsets before using this API.
//!
//! # Examples
//! ```
//! use libmodbuzz::DataModel;
//!
//! let mut model = DataModel::default();
//! model.write_coil(0, true);
//! assert_eq!(model.read_coil(0), Some(true));
//!
//! model.write_holding_register(0, 0x1234);
//! assert_eq!(model.read_holding_register(0), Some(0x1234));
//! ```

/// Default maximum number of discrete inputs (per Modbus spec).
pub const DEFAULT_MAX_DISCRETE_INPUTS: usize = 2000;
/// Default maximum number of coils (per Modbus spec).
pub const DEFAULT_MAX_COILS: usize = 2000;
/// Default maximum number of input registers (per Modbus spec).
pub const DEFAULT_MAX_INPUT_REGISTERS: usize = 125;
/// Default maximum number of holding registers (per Modbus spec).
pub const DEFAULT_MAX_HOLDING_REGISTERS: usize = 125;

/// Configuration for the Modbus data model limits.
///
/// Use this to create a data model with custom capacity. All fields default
/// to the standard Modbus specification limits.
///
/// # Examples
/// ```
/// use libmodbuzz::data_model::DataModelConfig;
/// use libmodbuzz::DataModel;
///
/// let config = DataModelConfig {
///     max_coils: 4000,
///     max_holding_registers: 1000,
///     ..Default::default()
/// };
/// let model = DataModel::with_config(config);
/// assert_eq!(model.max_coils(), 4000);
/// ```
#[derive(Debug, Clone)]
pub struct DataModelConfig {
    /// Maximum number of discrete inputs.
    pub max_discrete_inputs: usize,
    /// Maximum number of coils.
    pub max_coils: usize,
    /// Maximum number of input registers.
    pub max_input_registers: usize,
    /// Maximum number of holding registers.
    pub max_holding_registers: usize,
}

impl Default for DataModelConfig {
    fn default() -> Self {
        Self {
            max_discrete_inputs: DEFAULT_MAX_DISCRETE_INPUTS,
            max_coils: DEFAULT_MAX_COILS,
            max_input_registers: DEFAULT_MAX_INPUT_REGISTERS,
            max_holding_registers: DEFAULT_MAX_HOLDING_REGISTERS,
        }
    }
}

/// Modbus data model providing storage for all four Modbus data types.
///
/// Provides bounds-checked read/write access. Read operations return `Option`
/// to signal out-of-range accesses. Write operations silently ignore
/// out-of-range indices to match common Modbus device behavior.
#[derive(Debug, Clone)]
pub struct DataModel {
    discrete_inputs: Vec<bool>,
    coils: Vec<bool>,
    input_registers: Vec<u16>,
    holding_registers: Vec<u16>,
}

impl Default for DataModel {
    /// Creates a data model with standard Modbus limits, all values initialized
    /// to `false` / `0`.
    fn default() -> Self {
        Self::with_config(DataModelConfig::default())
    }
}

impl DataModel {
    /// Create a data model with custom configuration.
    ///
    /// All storage is allocated eagerly and initialized to `false` or `0`.
    pub fn with_config(config: DataModelConfig) -> Self {
        Self {
            discrete_inputs: vec![false; config.max_discrete_inputs],
            coils: vec![false; config.max_coils],
            input_registers: vec![0; config.max_input_registers],
            holding_registers: vec![0; config.max_holding_registers],
        }
    }

    // ── Capacity queries ──────────────────────────────────────────────

    /// Maximum number of discrete inputs this model can store.
    pub fn max_discrete_inputs(&self) -> usize {
        self.discrete_inputs.len()
    }

    /// Maximum number of coils this model can store.
    pub fn max_coils(&self) -> usize {
        self.coils.len()
    }

    /// Maximum number of input registers this model can store.
    pub fn max_input_registers(&self) -> usize {
        self.input_registers.len()
    }

    /// Maximum number of holding registers this model can store.
    pub fn max_holding_registers(&self) -> usize {
        self.holding_registers.len()
    }

    // ── Read operations ───────────────────────────────────────────────

    /// Read a discrete input value.
    ///
    /// Returns `None` if the index is out of range.
    pub fn read_discrete_input(&self, index: u16) -> Option<bool> {
        self.discrete_inputs.get(usize::from(index)).copied()
    }

    /// Read a coil value.
    ///
    /// Returns `None` if the index is out of range.
    pub fn read_coil(&self, index: u16) -> Option<bool> {
        self.coils.get(usize::from(index)).copied()
    }

    /// Read an input register value.
    ///
    /// Returns `None` if the index is out of range.
    pub fn read_input_register(&self, index: u16) -> Option<u16> {
        self.input_registers.get(usize::from(index)).copied()
    }

    /// Read a holding register value.
    ///
    /// Returns `None` if the index is out of range.
    pub fn read_holding_register(&self, index: u16) -> Option<u16> {
        self.holding_registers.get(usize::from(index)).copied()
    }

    // ── Write operations ──────────────────────────────────────────────

    /// Write a single coil value.
    ///
    /// Silently ignores writes to out-of-range indices.
    pub fn write_coil(&mut self, index: u16, value: bool) {
        if let Some(coil) = self.coils.get_mut(usize::from(index)) {
            *coil = value;
        }
    }

    /// Write a single holding register value.
    ///
    /// Silently ignores writes to out-of-range indices.
    pub fn write_holding_register(&mut self, index: u16, value: u16) {
        if let Some(reg) = self.holding_registers.get_mut(usize::from(index)) {
            *reg = value;
        }
    }

    /// Write multiple coil values starting at the given index.
    ///
    /// Writes as many values as fit within bounds; ignores the rest.
    pub fn write_multiple_coils(&mut self, start: u16, values: &[bool]) {
        let start = usize::from(start);
        for (i, &val) in values.iter().enumerate() {
            if let Some(coil) = self.coils.get_mut(start + i) {
                *coil = val;
            }
        }
    }

    /// Write multiple holding register values starting at the given index.
    ///
    /// Writes as many values as fit within bounds; ignores the rest.
    pub fn write_multiple_holding_registers(&mut self, start: u16, values: &[u16]) {
        let start = usize::from(start);
        for (i, &val) in values.iter().enumerate() {
            if let Some(reg) = self.holding_registers.get_mut(start + i) {
                *reg = val;
            }
        }
    }

    // ── Hardware/test setters (read-only data types) ──────────────────

    /// Set a discrete input value (for hardware drivers or testing).
    ///
    /// In a real device, discrete inputs are driven by hardware, not Modbus commands.
    pub fn set_discrete_input(&mut self, index: u16, value: bool) {
        if let Some(input) = self.discrete_inputs.get_mut(usize::from(index)) {
            *input = value;
        }
    }

    /// Set an input register value (for hardware drivers or testing).
    ///
    /// In a real device, input registers are driven by ADCs or sensors, not Modbus commands.
    pub fn set_input_register(&mut self, index: u16, value: u16) {
        if let Some(reg) = self.input_registers.get_mut(usize::from(index)) {
            *reg = value;
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn default_limits() {
        let model = DataModel::default();
        assert_eq!(model.max_coils(), 2000);
        assert_eq!(model.max_discrete_inputs(), 2000);
        assert_eq!(model.max_holding_registers(), 125);
        assert_eq!(model.max_input_registers(), 125);
    }

    #[test]
    fn custom_limits() {
        let config = DataModelConfig {
            max_coils: 100,
            max_holding_registers: 50,
            ..Default::default()
        };
        let model = DataModel::with_config(config);
        assert_eq!(model.max_coils(), 100);
        assert_eq!(model.max_holding_registers(), 50);
    }

    #[test]
    fn coil_read_write() {
        let mut model = DataModel::default();
        assert_eq!(model.read_coil(0), Some(false));
        model.write_coil(0, true);
        assert_eq!(model.read_coil(0), Some(true));
    }

    #[test]
    fn coil_out_of_range() {
        let model = DataModel::default();
        assert_eq!(model.read_coil(9999), None);
    }

    #[test]
    fn holding_register_read_write() {
        let mut model = DataModel::default();
        model.write_holding_register(0, 0x1234);
        assert_eq!(model.read_holding_register(0), Some(0x1234));
    }

    #[test]
    fn discrete_input_set() {
        let mut model = DataModel::default();
        model.set_discrete_input(5, true);
        assert_eq!(model.read_discrete_input(5), Some(true));
        assert_eq!(model.read_discrete_input(6), Some(false));
    }

    #[test]
    fn input_register_set() {
        let mut model = DataModel::default();
        model.set_input_register(0, 0xDEAD);
        assert_eq!(model.read_input_register(0), Some(0xDEAD));
    }

    #[test]
    fn write_multiple_coils() {
        let mut model = DataModel::default();
        model.write_multiple_coils(10, &[true, false, true]);
        assert_eq!(model.read_coil(10), Some(true));
        assert_eq!(model.read_coil(11), Some(false));
        assert_eq!(model.read_coil(12), Some(true));
    }

    #[test]
    fn write_multiple_registers() {
        let mut model = DataModel::default();
        model.write_multiple_holding_registers(5, &[0x1122, 0x3344]);
        assert_eq!(model.read_holding_register(5), Some(0x1122));
        assert_eq!(model.read_holding_register(6), Some(0x3344));
    }
}
