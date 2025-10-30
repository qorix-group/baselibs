/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

//! This module provides FFI-compatible types and functions to interoperate with score::Result and
//! associated types.
//!
//! It also offers integration with cxx, allowing type aliases of score::Result to be used as types
//! of arguments or return values of C++ functions and methods. See the documentation of
//! [`import_cpp_results!`] for an overview how to do this.

#[cfg(not(any(feature = "libcxx_layout", feature = "libstdcpp_layout")))]
compile_error!("Either feature \"libcxx_layout\" or \"libstdcpp_layout\" must be enabled to select the correct standard type layout");

mod ffi {
    use std::mem::ManuallyDrop;
    use std::ops::Deref;
    use std::fmt;

    // SAFETY: The caller must make sure that error_domain points to a valid error domain, and that
    // result points to a valid CStringView instance where the result will be written to.
    unsafe extern "C" {
        #[link_name = "LibResultErrorDomainGetMessageForErrorCode"]
        fn get_message_for_error_code(error_domain: *const ErrorDomain, error_code: ErrorCode, result: *mut CStringView);
    }

    /// Opaque type representing score::ErrorDomain
    ///
    /// Since we do not support creating ErrorDomains in Rust, we only need an opaque representation
    /// here since all usages are trough references and pointers.
    #[repr(C)]
    pub struct ErrorDomain {
        _dummy: [u8; 0],
    }

    impl ErrorDomain {
        /// Retrieve a human-readable message for a certain error of a domain.
        ///
        /// error_code must be a valid number for the respective domain, otherwise the result is
        /// undefined (but not unsafe - usually some default gets returned).
        pub fn get_message_for_error_code(&self, error_code: ErrorCode) -> CStringView {
            let mut error_domain_string = CStringView::default();
            // SAFETY: Since self is valid, and we provide a valid result string, calling this
            // function is safe.
            unsafe {
                get_message_for_error_code(self, error_code, &mut error_domain_string);
            }
            error_domain_string
        }
    }

    #[repr(C)]
    #[derive(Clone, Copy, Debug, PartialEq, Default)]
    #[cfg(feature = "libstdcpp_layout")]
    struct CStringViewStorage {
        len: libc::size_t,
        data: *const std::ffi::c_char,
    }

    #[repr(C)]
    #[derive(Clone, Copy, Debug, PartialEq, Default)]
    #[cfg(feature = "libcxx_layout")]
    struct CStringViewStorage {
        data: *const std::ffi::c_char,
        len: libc::size_t,
    }

    /// Binary compatible Rust version of `std::string_view`
    ///
    /// This type can be safely sent across FFI boundaries where `std::string_view` is expected. It
    /// offers conversions to Rust string types, assuming a UTF-8 encoding. It also offers
    /// conversions to byte slices, so if any other encoding is used, the caller may use this slice
    /// and covert it themselves.
    ///
    /// Assumptions on the layout:
    /// * Length is of C++ type `std::size_t`
    /// * Characters are of type `char` (i.e. [`std::ffi::c_char`] in Rust)
    /// * On some platforms (e.g. QNX with libcxx), the field order is swapped
    #[repr(transparent)]
    #[derive(Clone, Copy, Debug, PartialEq, Default)]
    pub struct CStringView {
        storage: CStringViewStorage
    }

    impl CStringView {
        fn as_char_slice(&self) -> &[std::ffi::c_char] {
            if self.storage.len > 0 {
                assert!(!self.storage.data.is_null());
                // SAFETY: We checked that data is not null and len > 0. Everything else is
                // accounted by either the fact that this came across FFI (which is unsafe anyway)
                // or by the careful caller of from_parts (which is unsafe as well).
                unsafe { std::slice::from_raw_parts(self.storage.data, self.storage.len) }
            } else {
                &[]
            }
        }

        fn as_byte_slice(&self) -> &[u8] {
            assert_eq!(std::mem::size_of::<std::ffi::c_char>(), std::mem::size_of::<u8>(),
                       "c_char must be of the same size as u8.");
            assert_eq!(std::mem::align_of::<std::ffi::c_char>(), std::mem::align_of::<u8>(),
                       "c_char must be of the same alignment as u8.");
            if self.storage.len > 0 {
                assert!(!self.storage.data.is_null(), "String data is null but length is non-zero");
                // SAFETY: This is safe because c_char is guaranteed to have the same size and
                // alignment as u8.
                unsafe { &*(self.as_char_slice() as *const [std::ffi::c_char] as *const [u8]) }
            } else {
                &[]
            }
        }

        /// Create a string view from a pointer to C chars and a length.
        ///
        /// # Panics
        /// This function will panic if `data` is null.
        ///
        /// # Safety
        /// The caller must ensure that `data` points to a valid memory region of at least `len`
        /// bytes, and that this memory remains valid for the lifetime of the returned
        /// `CStringView`.
        pub unsafe fn from_parts(data: *const std::ffi::c_char, len: usize) -> Self {
            assert!(!data.is_null());
            Self { storage: CStringViewStorage { data, len: len as libc::size_t } }
        }

        /// Convert to a UTF-8 Rust string slice.
        ///
        /// This method uses [`std::str::from_utf8`] internally, so all the same rules apply.
        ///
        /// # Panics
        /// This function will panic if `data` is null.
        pub fn to_str(&self) -> std::result::Result<&str, std::str::Utf8Error> {
            std::str::from_utf8(self.as_ref())
        }

        /// Convert to a UTF-8 Rust string slice, replacing invalid sequences with U+FFFD.
        ///
        /// This method uses [`std::string::String::from_utf8_lossy`] internally, so all the same
        /// rules apply.
        ///
        /// # Panics
        /// This function will panic if `data` is null.
        pub fn to_string_lossy(&self) -> std::borrow::Cow<'_, str> {
            String::from_utf8_lossy(self.as_ref())
        }
    }

    impl Deref for CStringView {
        type Target = [std::ffi::c_char];

        fn deref(&self) -> &Self::Target {
            self.as_char_slice()
        }
    }

    impl AsRef<[u8]> for CStringView {
        fn as_ref(&self) -> &[u8] {
            self.as_byte_slice()
        }
    }

    impl fmt::Display for CStringView {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> fmt::Result {
            f.write_str(self.to_string_lossy().as_ref())
        }
    }

    /// Rust representation of score::result::ErrorCode
    pub type ErrorCode = i32;

    #[repr(C)]
    #[derive(Clone, Copy, Debug, PartialEq)]
    /// Binary compatible representation of `score::result::Error`
    ///
    /// This type can be safely sent across FFI boundaries where `score::result::Error` is expected.
    pub struct Error {
        code: ErrorCode,
        domain: *const ErrorDomain,
        message: CStringView,
    }

    impl fmt::Display for Error {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> fmt::Result {
            // SAFETY: We can assume that domain is valid as long as the Error instance is valid
            // (type invariant).
            let error_domain_string = unsafe {
                self.domain
                    .as_ref()
                    .expect("Domain is nullptr!")
                    .get_message_for_error_code(self.code)
            };
            write!(f, "Error {} ({}): {}", error_domain_string, self.code, self.message)
        }
    }

    impl Error {
        /// Create an Error from its parts, namely. This is unsafe because the caller must ensure
        /// that the `domain` pointer is valid for the lifetime of the Error instance.
        ///
        /// # Safety
        /// The caller must ensure that `domain` points to a valid score::error:ErrorDomain instance
        /// that lives at least as long as the returned Error instance.
        pub unsafe fn from_parts(code: ErrorCode, domain: *const ErrorDomain, message: CStringView) -> Self {
            Self { code, domain, message, }
        }

        /// Returns the error code.
        pub fn code(&self) -> ErrorCode {
            self.code
        }

        /// Returns the error message given by the user during creation.
        pub fn message(&self) -> CStringView {
            self.message
        }

        /// Returns the error domain.
        ///
        /// # Panics
        /// This function will panic if the domain is null.
        pub fn domain(&self) -> &ErrorDomain {
            // SAFETY: This is safe as long as the user of from_parts ensured that domain is valid,
            // or because the Error was created in C++ and passed to Rust in a valid way.
            unsafe { self.domain.as_ref().expect("Domain not set") }
        }
    }

    impl std::error::Error for Error {}

    #[repr(u8)]
    #[derive(Debug, PartialEq, Copy, Clone)]
    enum ExpectedDiscriminant {
        Value = 0,
        Error = 1,
        Stale = 255, // HAXX: Stale means that this is a moved-from result
    }

    #[repr(C)]
    union ExpectedStorage<T, E> {
        value: std::mem::ManuallyDrop<T>,
        error: std::mem::ManuallyDrop<E>,
    }

    #[repr(C)]
    /// Binary compatible representation of score::details::Expected<T, E>
    ///
    /// This is the foundation of `score::Result<T>`. It allows for storing either a value or an error.
    /// It can be used with any T across FFI that is also FFI safe, meaning it can be trivially
    /// moved and correctly handles ownership and cleanup.
    ///
    /// The main feature of this class besides FFI compatibility is that it is convertible to
    /// `std::result::Result<T, E>`, allowing for easy interop with Rust code. This is implemented
    /// by the `Into<std::result::Result<T, E>>` trait. For code that receives a `Result`, you
    /// would usually then write something like that:
    ///
    /// ```ignore
    /// let cpp_result = unsafe { ffi::cpp_function() }; // Returns ffi::Result<T>
    /// let rust_result: std::result::Result<T, ffi::Error> = cpp_result.into();
    /// match rust_result {
    ///     Ok(value) => // ...,
    ///     Err(err) => // ...,
    /// }
    /// ```
    ///
    /// Assumptions on the layout of expected:
    /// * The actual data comes first
    /// * The discriminant is u8
    /// * Discriminant = 0 means "value"
    /// * Discriminant = 1 means "error"
    ///
    /// Discriminant = 255 is not used by C++ as it's used on the Rust side to mark a moved-from
    /// result
    //
    // # Safety
    //
    // The invariant of this class is that the discriminant always correctly indicates which of the
    // union members is currently valid. If the discriminant is Stale, then neither member is valid.
    // If this invariant is violated, the behavior is undefined and any safety argumentation become
    // void.
    pub struct Expected<T, E> {
        storage: ExpectedStorage<T, E>,
        discriminant: ExpectedDiscriminant,
    }

    impl<T, E> Drop for Expected<T, E> {
        fn drop(&mut self) {
            match self.discriminant {
                // SAFETY: We only manually drop if the discriminant indicates the existence of
                // either a value or an error. In the stale case, nothing will be done.
                ExpectedDiscriminant::Value =>
                    unsafe { std::mem::ManuallyDrop::drop(&mut self.storage.value) },
                ExpectedDiscriminant::Error => unsafe { std::mem::ManuallyDrop::drop(&mut self.storage.error) },
                ExpectedDiscriminant::Stale => () // Do nothing, this is a moved-from result
            }
        }
    }

    impl<T, E> Expected<T, E> {
        /// Create an Expected instance containing a value.
        pub fn from_value(value: T) -> Self {
            Self {
                storage: ExpectedStorage { value: ManuallyDrop::new(value) },
                discriminant: ExpectedDiscriminant::Value,
            }
        }

        /// Create an Expected instance containing an error.
        pub fn from_error(error: E) -> Self {
            Self {
                storage: ExpectedStorage { error: ManuallyDrop::new(error) },
                discriminant: ExpectedDiscriminant::Error,
            }
        }

        /// Returns true if this instance contains a value, false if it contains an error.
        pub fn has_value(&self) -> bool {
            match self.discriminant {
                ExpectedDiscriminant::Value => true,
                ExpectedDiscriminant::Error => false,
                ExpectedDiscriminant::Stale => unreachable!("has_value called on a moved-from value"),
            }
        }

        /// Returns a reference to the contained value, or None if this instance contains an error.
        pub fn get_value(&self) -> Option<&T> {
            match self.discriminant {
                // SAFETY: We only access the value part of the union if the discriminant indicates
                // the presence of this value.
                ExpectedDiscriminant::Value => Some(unsafe { self.storage.value.deref() }),
                ExpectedDiscriminant::Error => None,
                ExpectedDiscriminant::Stale => unreachable!("get_value called on a moved-from value"),
            }
        }

        /// Returns a reference to the contained error, or None if this instance contains a value.
        pub fn get_error(&self) -> Option<&E> {
            match self.discriminant {
                ExpectedDiscriminant::Value => None,
                // SAFETY: We only access the error part of the union if the discriminant indicates
                // the presence of such an error.
                ExpectedDiscriminant::Error => Some(unsafe { self.storage.error.deref() }),
                ExpectedDiscriminant::Stale => unreachable!("get_error called on a moved-from value"),
            }
        }
    }

    impl<T: Clone, E: Clone> Clone for Expected<T, E> {
        fn clone(&self) -> Self {
            if let Some(error) = self.get_error() {
                Self::from_error(error.clone())
            } else if let Some(value) = self.get_value() {
                Self::from_value(value.clone())
            } else {
                unreachable!("clone called on a moved-from value")
            }
        }
    }

    impl<T: fmt::Debug, E: fmt::Debug> fmt::Debug for Expected<T, E> {
        fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
            match self.discriminant {
                ExpectedDiscriminant::Value => {
                    let value = self.get_value();
                    f.debug_tuple("Expected::Value").field(&value).finish()
                }
                ExpectedDiscriminant::Error => {
                    let error = self.get_error();
                    f.debug_tuple("Expected::Error").field(&error).finish()
                }
                ExpectedDiscriminant::Stale => write!(f, "Expected::Stale (moved-from)"),
            }
        }
    }

    impl<T: PartialEq, E: PartialEq> PartialEq for Expected<T, E> {
        fn eq(&self, other: &Self) -> bool {
            if self.discriminant != other.discriminant {
                return false;
            }
            // SAFETY: Since we only use the union members that fit the discriminant, this is safe.
            match self.discriminant {
                ExpectedDiscriminant::Value => {
                    let self_value = unsafe { self.storage.value.deref() };
                    let other_value = unsafe { other.storage.value.deref() };
                    self_value == other_value
                }
                ExpectedDiscriminant::Error => {
                    let self_error = unsafe { self.storage.error.deref() };
                    let other_error = unsafe { other.storage.error.deref() };
                    self_error == other_error
                }
                ExpectedDiscriminant::Stale => true, // Both are stale, consider equal
            }
        }
    }

    impl<T: Eq, E: Eq> Eq for Expected<T, E> {}

    impl<T, E> From<Expected<T, E>> for std::result::Result<T, E> {
        fn from(mut expected: Expected<T, E>) -> Self {
            // SAFETY: Since we only use the union members that fit the discriminant, this is safe.
            let result = match expected.discriminant {
                ExpectedDiscriminant::Value => Ok(unsafe { ManuallyDrop::take(&mut expected.storage.value) }),
                ExpectedDiscriminant::Error => Err(unsafe { ManuallyDrop::take(&mut expected.storage.error) }),
                ExpectedDiscriminant::Stale => unreachable!("from called on a moved-from value"),
            };
            // Signal to the drop implementation that this has been moved from
            expected.discriminant = ExpectedDiscriminant::Stale;
            result
        }
    }

    /// This is the main type to be used for FFI interop with `score::Result<T>`. It is a type alias
    /// for `Expected<T, score::result::Error>`. See the documentation of [`Expected`] for more
    /// details.
    pub type Result<T> = Expected<T, Error>;

    impl<T> From<T> for Result<T> {
        fn from(value: T) -> Self {
            Self::from_value(value)
        }
    }

    #[cfg(test)]
    mod tests {
        use super::*;
        use std::mem;
        use std::ptr;

        // External C functions that work with real score::Result instances
        extern "C" {
            fn create_score_result_success_int32(value: i32) -> *mut std::ffi::c_void;
            fn create_score_result_error_int32(error_code: i32, message: *const std::ffi::c_char) -> *mut std::ffi::c_void;
            fn create_score_result_success_bool(value: bool) -> *mut std::ffi::c_void;
            fn create_score_result_error_bool(error_code: i32, message: *const std::ffi::c_char) -> *mut std::ffi::c_void;

            fn destroy_score_result_int32(result: *mut std::ffi::c_void);
            fn destroy_score_result_bool(result: *mut std::ffi::c_void);

            fn get_score_result_int32_size() -> libc::size_t;
            fn get_score_result_bool_size() -> libc::size_t;
            fn get_score_error_size() -> libc::size_t;

            fn verify_rust_result_as_cpp_int32(
                rust_result: *const std::ffi::c_void,
                should_have_value: bool,
                expected_value_or_error_code: i32
            ) -> bool;

            fn print_score_result_memory_layout();

            // Function to create and test std::string_view layout compatibility
            fn create_cpp_string_view(data: *const std::ffi::c_char, len: libc::size_t) -> *mut std::ffi::c_void;
            fn create_empty_cpp_string_view() -> *mut std::ffi::c_void;
            fn destroy_cpp_string_view(sv: *mut std::ffi::c_void);
            fn get_cpp_string_view_size() -> libc::size_t;
            fn verify_string_view_layout(rust_view: *const std::ffi::c_void, expected_data: *const std::ffi::c_char, expected_len: libc::size_t) -> bool;
        }

        #[test]
        fn test_cstring_view_layout_compatibility() {
            unsafe {
                // Test that CStringView has the same memory layout as std::string_view
                let test_str = "Hello, FFI World!";
                let c_str = std::ffi::CString::new(test_str).unwrap();

                // Create our Rust CStringView
                let rust_string_view = CStringView::from_parts(c_str.as_ptr(), test_str.len());

                println!("\n=== CStringView Layout Verification ===");
                println!("Test string: \"{}\"", test_str);
                println!("String length: {}", test_str.len());

                // Verify size compatibility
                let cpp_string_view_size = get_cpp_string_view_size();
                let rust_string_view_size = mem::size_of::<CStringView>();

                println!("Size comparison:");
                println!("  C++ std::string_view: {} bytes", cpp_string_view_size);
                println!("  Rust CStringView:     {} bytes", rust_string_view_size);
                println!("  Sizes match: {}", cpp_string_view_size == rust_string_view_size);

                assert_eq!(cpp_string_view_size, rust_string_view_size,
                          "CStringView size must match std::string_view size");

                // Verify field layout by checking memory positions
                let rust_view_ptr = &rust_string_view as *const CStringView as *const std::ffi::c_void;

                // Test that C++ can correctly interpret our Rust CStringView
                let layout_compatible = verify_string_view_layout(
                    rust_view_ptr,
                    c_str.as_ptr(),
                    test_str.len() as libc::size_t
                );

                println!("Layout compatibility:");
                println!("  C++ can read Rust CStringView: {}", layout_compatible);

                assert!(layout_compatible,
                       "C++ should be able to correctly read Rust CStringView fields");

                // Create C++ string_view and verify we can read it correctly
                let cpp_string_view = create_cpp_string_view(c_str.as_ptr(), test_str.len() as libc::size_t);
                assert!(!cpp_string_view.is_null(), "C++ should create valid string_view");

                let rust_read_cpp_view = &*(cpp_string_view as *const CStringView);

                println!("Reverse compatibility:");
                println!("  Rust reading C++ string_view:");
                println!("    Length: {} (expected: {})", rust_read_cpp_view.storage.len, test_str.len());
                println!("    Data ptr match: {}", rust_read_cpp_view.storage.data == c_str.as_ptr());

                assert_eq!(rust_read_cpp_view.storage.len as usize, test_str.len(),
                          "Length field should match when reading C++ string_view from Rust");
                assert_eq!(rust_read_cpp_view.storage.data, c_str.as_ptr(),
                          "Data pointer should match when reading C++ string_view from Rust");

                // Verify we can convert the C++ created string_view to a Rust string
                let converted_str = rust_read_cpp_view.to_str().expect("Should convert to valid UTF-8");
                println!("    Converted string: \"{}\"", converted_str);
                assert_eq!(converted_str, test_str, "Converted string should match original");

                // Clean up
                destroy_cpp_string_view(cpp_string_view);

                println!("✓ CStringView layout is compatible with std::string_view");
            }
        }

        #[test]
        fn test_equality_of_defaulted_string_instances() {
            unsafe {
                let cpp_created_string_view = (create_empty_cpp_string_view() as *mut CStringView).as_mut().unwrap();
                let default_cstringview = CStringView::default();
                assert_eq!(cpp_created_string_view.storage.len, default_cstringview.storage.len,
                           "Lengths of default CStringView and C++ created empty string_view should match");
                assert_eq!(cpp_created_string_view.storage.data, default_cstringview.storage.data,
                           "Data pointers of default CStringView and C++ created empty string_view should match");
                destroy_cpp_string_view(cpp_created_string_view as *mut _ as *mut libc::c_void);
            }
        }

        #[test]
        fn test_cstring_view_field_offsets() {
            // This test specifically verifies our layout assumptions about field ordering
            println!("\n=== CStringView Field Layout Analysis ===");

            // Check field offsets
            let len_offset = mem::offset_of!(CStringViewStorage, len);
            let data_offset = mem::offset_of!(CStringViewStorage, data);

            println!("Field offsets:");
            println!("  len field:  {} bytes from start", len_offset);
            println!("  data field: {} bytes from start", data_offset);

            // Verify field ordering based on feature flag
            #[cfg(feature = "libcxx_layout")]
            {
                println!("  Using QNX layout (data first, then length)");
                // QNX layout: data pointer comes first, then length
                assert_eq!(data_offset, 0, "Data field should be at offset 0 (first field) in QNX layout");
                assert_eq!(len_offset, mem::size_of::<*const std::ffi::c_char>(),
                          "Length field should come after data field in QNX layout");

                println!("  ✓ Data field is first (offset 0)");
                println!("  ✓ Length field is second (offset {})", len_offset);
            }

            #[cfg(feature = "libstdcpp_layout")]
            {
                println!("  Using standard layout (length first, then data)");
                // Standard layout: length comes first, then data pointer
                assert_eq!(len_offset, 0, "Length field should be at offset 0 (first field) in standard layout");
                assert_eq!(data_offset, mem::size_of::<libc::size_t>(),
                          "Data field should come after length field in standard layout");

                println!("  ✓ Length field is first (offset 0)");
                println!("  ✓ Data field is second (offset {})", data_offset);
            }

            // Verify total size is as expected
            let expected_size = mem::size_of::<libc::size_t>() + mem::size_of::<*const std::ffi::c_char>();
            let actual_size = mem::size_of::<CStringView>();

            println!("Size analysis:");
            println!("  size_t: {} bytes", mem::size_of::<libc::size_t>());
            println!("  char*:  {} bytes", mem::size_of::<*const std::ffi::c_char>());
            println!("  Expected CStringView size: {} bytes", expected_size);
            println!("  Actual CStringView size:   {} bytes", actual_size);

            // Note: actual size might be larger due to padding/alignment
            assert!(actual_size >= expected_size,
                   "CStringView should be at least as large as its constituent fields");

            println!("  ✓ Size is consistent with field layout");

            // Test with actual data to verify field access
            let test_data = "Field offset test";
            let c_str = std::ffi::CString::new(test_data).unwrap();
            let string_view = unsafe {
                CStringView::from_parts(c_str.as_ptr(), test_data.len())
            };

            // Access fields and verify they contain expected values
            assert_eq!(string_view.storage.len, test_data.len());
            assert_eq!(string_view.storage.data, c_str.as_ptr());

            // Verify field access through raw pointer manipulation
            let view_ptr = &string_view as *const CStringView;
            unsafe {
                #[cfg(feature = "libcxx_layout")]
                {
                    let data_ptr = view_ptr as *const *const std::ffi::c_char;
                    let len_ptr = (view_ptr as *const u8).add(len_offset) as *const libc::size_t;

                    let data_via_offset = *data_ptr;
                    let len_via_offset = *len_ptr;

                    println!("Direct field access verification (QNX layout):");
                    println!("  Data ptr via offset matches: {}", data_via_offset == c_str.as_ptr());
                    println!("  Length via offset: {} (expected: {})", len_via_offset, test_data.len());

                    assert_eq!(data_via_offset, c_str.as_ptr());
                    assert_eq!(len_via_offset as usize, test_data.len());
                }

                #[cfg(feature = "libstdcpp_layout")]
                {
                    let len_ptr = view_ptr as *const libc::size_t;
                    let data_ptr = (view_ptr as *const u8).add(data_offset) as *const *const std::ffi::c_char;

                    let len_via_offset = *len_ptr;
                    let data_via_offset = *data_ptr;

                    println!("Direct field access verification (standard layout):");
                    println!("  Length via offset: {} (expected: {})", len_via_offset, test_data.len());
                    println!("  Data ptr via offset matches: {}", data_via_offset == c_str.as_ptr());

                    assert_eq!(len_via_offset, test_data.len());
                    assert_eq!(data_via_offset, c_str.as_ptr());
                }
            }

            #[cfg(feature = "libcxx_layout")]
            println!("✓ libcxx field layout assumptions verified");

            #[cfg(feature = "libstdcpp_layout")]
            println!("✓ libstdc++ field layout assumptions verified");
        }

        #[test]
        fn test_binary_compatibility_sizes() {
            unsafe {
                let cpp_result_int32_size = get_score_result_int32_size();
                let cpp_result_bool_size = get_score_result_bool_size();
                let cpp_error_size = get_score_error_size();

                let rust_result_int32_size = mem::size_of::<Result<i32>>();
                let rust_result_bool_size = mem::size_of::<Result<bool>>();
                let rust_error_size = mem::size_of::<Error>();

                println!("\n=== Size Compatibility Analysis ===");
                println!("Result<i32>:");
                println!("  C++ score::Result<int32_t>: {} bytes", cpp_result_int32_size);
                println!("  Rust ffi::Result<i32>:    {} bytes", rust_result_int32_size);
                println!("  Compatible: {}", cpp_result_int32_size == rust_result_int32_size);

                println!("\nResult<bool>:");
                println!("  C++ score::Result<bool>:    {} bytes", cpp_result_bool_size);
                println!("  Rust ffi::Result<bool>:   {} bytes", rust_result_bool_size);
                println!("  Compatible: {}", cpp_result_bool_size == rust_result_bool_size);

                println!("\nError:");
                println!("  C++ score::result::Error:   {} bytes", cpp_error_size);
                println!("  Rust ffi::Error:          {} bytes", rust_error_size);
                println!("  Compatible: {}", cpp_error_size == rust_error_size);

                // Print C++ memory layout for reference
                print_score_result_memory_layout();

                // These assertions will fail if our assumptions are wrong
                assert_eq!(cpp_result_int32_size, rust_result_int32_size,
                          "Size mismatch between C++ score::Result<int32_t> and Rust Result<i32>");
                // Note: We might need to relax this for Error and bool Result due to different layouts
            }
        }

        #[test]
        fn test_cpp_created_results_in_rust() {
            unsafe {
                let test_message = std::ffi::CString::new("Test error from C++").unwrap();

                // Test C++ created success result
                let cpp_success = create_score_result_success_int32(12345);
                assert!(!cpp_success.is_null(), "C++ should have created a valid success result");

                // Cast to our Rust type and test
                let rust_success = &*(cpp_success as *const Result<i32>);
                println!("C++ created success result analysis:");
                println!("  has_value(): {}", rust_success.has_value());

                if rust_success.has_value() {
                    let value = rust_success.get_value().unwrap();
                    println!("  value: {}", value);
                    assert_eq!(*value, 12345, "Value should match what was set in C++");
                } else {
                    panic!("C++ created success result should have a value when viewed from Rust");
                }

                // Test C++ created error result
                let cpp_error = create_score_result_error_int32(54321, test_message.as_ptr());
                assert!(!cpp_error.is_null(), "C++ should have created a valid error result");

                let rust_error = &*(cpp_error as *const Result<i32>);
                println!("\nC++ created error result analysis:");
                println!("  has_value(): {}", rust_error.has_value());

                if !rust_error.has_value() {
                    let error = rust_error.get_error().unwrap();
                    println!("  error code: {}", error.code);
                    assert_eq!(error.code, 54321, "Error code should match what was set in C++");
                    let friendly_error_message = error.to_string();
                    assert_eq!(&friendly_error_message,
                               "Error Test error from C++ (54321): Test error from C++");
                } else {
                    panic!("C++ created error result should have an error when viewed from Rust");
                }

                // Clean up
                destroy_score_result_int32(cpp_success);
                destroy_score_result_int32(cpp_error);
            }
        }

        #[test]
        fn test_rust_created_results_in_cpp() {
            unsafe {
                // Create success result in Rust
                let rust_success = Result::from(67890i32);
                let rust_success_ptr = &rust_success as *const Result<i32> as *const std::ffi::c_void;

                println!("\nRust created success result analysis:");
                let cpp_verified = verify_rust_result_as_cpp_int32(rust_success_ptr, true, 67890);
                println!("  C++ verification passed: {}", cpp_verified);
                assert!(cpp_verified, "C++ should be able to read Rust-created success result");

                // Create error result in Rust
                let test_str = "Rust created error";
                let c_str = std::ffi::CString::new(test_str).unwrap();
                let string_view = CStringView {
                    storage: CStringViewStorage {
                        len: test_str.len(),
                        data: c_str.as_ptr(),
                    }
                };

                let error = Error {
                    code: 98765,
                    domain: ptr::null(),
                    message: string_view,
                };

                let rust_error = Result::<i32>::from_error(error);
                let rust_error_ptr = &rust_error as *const Result<i32> as *const std::ffi::c_void;

                println!("\nRust created error result analysis:");
                let cpp_error_verified = verify_rust_result_as_cpp_int32(rust_error_ptr, false, 98765);
                println!("  C++ verification passed: {}", cpp_error_verified);
                assert!(cpp_error_verified, "C++ should be able to read Rust-created error result");
            }
        }

        #[test]
        fn test_memory_layout_assumptions() {
            // This test verifies our layout assumptions are reasonable
            println!("\n=== Rust FFI Layout Assumptions ===");
            println!("Basic types:");
            println!("  i32: {} bytes, {} align", mem::size_of::<i32>(), mem::align_of::<i32>());
            println!("  u8: {} bytes, {} align", mem::size_of::<u8>(), mem::align_of::<u8>());
            println!("  *const c_void: {} bytes, {} align", mem::size_of::<*const std::ffi::c_void>(), mem::align_of::<*const std::ffi::c_void>());
            println!("  size_t: {} bytes, {} align", mem::size_of::<libc::size_t>(), mem::align_of::<libc::size_t>());

            println!("\nFFI types:");
            println!("  CStringView: {} bytes, {} align", mem::size_of::<CStringView>(), mem::align_of::<CStringView>());
            println!("  Error: {} bytes, {} align", mem::size_of::<Error>(), mem::align_of::<Error>());
            println!("  Expected<i32, Error>: {} bytes, {} align", mem::size_of::<Expected<i32, Error>>(), mem::align_of::<Expected<i32, Error>>());
            println!("  Result<i32>: {} bytes, {} align", mem::size_of::<Result<i32>>(), mem::align_of::<Result<i32>>());

            // Field offset analysis
            println!("\nField offsets:");
            println!("  CStringView.len: {}", mem::offset_of!(CStringViewStorage, len));
            println!("  CStringView.data: {}", mem::offset_of!(CStringViewStorage, data));
            println!("  Error.code: {}", mem::offset_of!(Error, code));
            println!("  Error.domain: {}", mem::offset_of!(Error, domain));
            println!("  Error.message: {}", mem::offset_of!(Error, message));
            println!("  Expected.storage: {}", mem::offset_of!(Expected<i32, Error>, storage));
            println!("  Expected.discriminant: {}", mem::offset_of!(Expected<i32, Error>, discriminant));

            // Verify discriminant values
            assert_eq!(ExpectedDiscriminant::Value as u8, 0);
            assert_eq!(ExpectedDiscriminant::Error as u8, 1);
            assert_eq!(ExpectedDiscriminant::Stale as u8, 255);

            println!("\nDiscriminant values:");
            println!("  Value: {}", ExpectedDiscriminant::Value as u8);
            println!("  Error: {}", ExpectedDiscriminant::Error as u8);
            println!("  Stale: {}", ExpectedDiscriminant::Stale as u8);
        }

        #[test]
        fn test_round_trip_compatibility() {
            unsafe {
                // Test different value types to see if our assumptions hold across different sizes

                // Test with bool (smaller than i32)
                let test_message = std::ffi::CString::new("Bool test error").unwrap();

                let cpp_bool_success = create_score_result_success_bool(true);
                let rust_bool_success = &*(cpp_bool_success as *const Result<bool>);

                println!("\nBool result compatibility:");
                println!("  C++ created bool success has_value: {}", rust_bool_success.has_value());

                if rust_bool_success.has_value() {
                    let value = rust_bool_success.get_value().unwrap();
                    println!("  Bool value: {}", value);
                    assert!(*value);
                }

                let cpp_bool_error = create_score_result_error_bool(111, test_message.as_ptr());
                let rust_bool_error = &*(cpp_bool_error as *const Result<bool>);

                println!("  C++ created bool error has_value: {}", rust_bool_error.has_value());

                if !rust_bool_error.has_value() {
                    let error = rust_bool_error.get_error().unwrap();
                    println!("  Bool error code: {}", error.code);
                    assert_eq!(error.code, 111);
                }

                // Clean up
                destroy_score_result_bool(cpp_bool_success);
                destroy_score_result_bool(cpp_bool_error);
            }
        }

        #[test]
        fn test_discriminant_interpretation() {
            // Create Rust results and verify discriminant values are as expected
            let success_result = Result::from(42i32);
            let error_result = {
                let test_str = "Discriminant test error";
                let c_str = std::ffi::CString::new(test_str).unwrap();
                let string_view = CStringView {
                    storage: CStringViewStorage {
                        len: test_str.len(),
                        data: c_str.as_ptr(),
                    }
                };
                let error = Error {
                    code: 123,
                    domain: ptr::null(),
                    message: string_view,
                };
                Result::<i32>::from_error(error)
            };

            // Check discriminant values directly
            let success_discriminant = success_result.discriminant;
            let error_discriminant = error_result.discriminant;

            println!("\nDiscriminant verification:");
            println!("  Success result discriminant: {} (should be 0)", success_discriminant as u8);
            println!("  Error result discriminant: {} (should be 1)", error_discriminant as u8);

            assert_eq!(success_discriminant as u8, 0);
            assert_eq!(error_discriminant as u8, 1);

            // Verify the actual functionality matches the discriminant
            assert!(success_result.has_value());
            assert!(!error_result.has_value());
        }
    }
}

pub use ffi::{CStringView, Expected, Result, Error};

#[macro_export]
#[doc(hidden)]
macro_rules! import_result {
    ($name:ident, $typename:ty, $cxxid:tt) => {
        #[repr(transparent)]
        struct $name($crate::Result<$typename>);

        impl From<$name> for ::std::result::Result<$typename, $crate::Error> {
            fn from(cpp_result: $name) -> Self {
                Self::from(cpp_result.0)
            }
        }

        // SAFETY: The calling macro forces the user to declare the type as unsafe. We pass this
        // to the `ExternType` trait.
        unsafe impl cxx::ExternType for $name {
            type Id = cxx::type_id!($cxxid);
            type Kind = cxx::kind::Trivial;
        }
    }
}

#[macro_export]
/// Declare Result types for use with cxx.
///
/// Usage of this macro looks like this:
///
/// ```ignore
/// import_cpp_results! {
///     unsafe type <Rust type name> = Result<<Rust type>> as "<ns1::ns2::CppTypeName>";
/// }
/// ```
///
/// # Example usage
///
/// For types on C++ side that shall be usable with cxx, we need to generate some extra boilerplate.
/// Since score::Result isn't directly integrated into cxx, we need to make cxx familiar with the FFI
/// of score::Result _for each distinct type_.
///
/// For example, importing two C++ that look like this on C++ side
///
/// ```c++
/// using Int32Result = score::Result<int32_t>;
/// using UniquePtrResult = score::Result<std::unique_ptr<Storage>>;
///
/// ```
/// ...macro usage on Rust side would look like this:
///
/// ```ignore
/// import_cpp_results! {
///     unsafe type ResultInt32 = Result<i32> as "Int32Result";
///     unsafe type ResultUniquePtrStorage = Result<UniquePtr<Storage>> as "UniquePtrResult";
/// }
///
/// #[cxx::bridge]
/// mod ffi {
///     type ResultInt32 = super::ResultInt32;
///     type ResultUniquePtrStorage = super::ResultUniquePtrStorage;
///
///     extern "C++" {
///         unsafe fn cpp_function(values: CxxVector<u32>) -> ResultInt32;
///
///         type Storage;
///         unsafe fn cpp_create_storage(file: &str) -> ResultUniquePtrStorage;
///     }
/// }
///```
///
/// # Safety
///
/// Declaring these types is inherently unsafe for multiple reasons:
/// 1. The compiler cannot know whether the type on C++ side really is a Result<T> with the same T
///    as on Rust side.
/// 2. The compiler cannot verify whether the type is trivial and thus relocatable. While there is a
///    check on C++ side inside the generated code, we cannot be sure whether this check gets
///    overridden in an invalid way.
///
macro_rules! import_cpp_results {
    { $(unsafe type $name:ident = Result<$typename:ty> as $cxxid:tt;)* } => {
        $($crate::import_result!($name, $typename, $cxxid);)*
    }
}

#[cfg(test)]
mod main_module_tests {
    use super::ffi::{Result, Error, CStringView};
    use std::ptr;

    // External C functions that work with real score::Result instances
    extern "C" {
        fn create_score_result_success_int32(value: i32) -> *mut std::ffi::c_void;
        fn create_score_result_error_int32(error_code: i32, message: *const std::ffi::c_char) -> *mut std::ffi::c_void;
        fn create_score_result_success_bool(value: bool) -> *mut std::ffi::c_void;
        fn create_score_result_error_bool(error_code: i32, message: *const std::ffi::c_char) -> *mut std::ffi::c_void;

        fn destroy_score_result_int32(result: *mut std::ffi::c_void);
        fn destroy_score_result_bool(result: *mut std::ffi::c_void);
    }

    #[test]
    fn test_cpp_result_to_std_result_success() {
        unsafe {
            // Create a success result on C++ side
            let cpp_success = create_score_result_success_int32(42);
            assert!(!cpp_success.is_null(), "C++ should create a valid success result");

            // Cast to our Rust Result type
            let rust_result = &*(cpp_success as *const Result<i32>);

            // Convert to std::result::Result
            let std_result: std::result::Result<i32, Error> =
                std::ptr::read(rust_result).into();

            println!("C++ success -> std::result::Result conversion:");
            println!("  std_result.is_ok(): {}", std_result.is_ok());

            assert!(std_result.is_ok(), "Converted result should be Ok");
            assert_eq!(std_result.unwrap(), 42, "Value should match C++ created value");

            // Clean up
            destroy_score_result_int32(cpp_success);
        }
    }

    #[test]
    fn test_cpp_result_to_std_result_error() {
        unsafe {
            let test_message = std::ffi::CString::new("C++ error for std conversion").unwrap();

            // Create an error result on C++ side
            let cpp_error = create_score_result_error_int32(999, test_message.as_ptr());
            assert!(!cpp_error.is_null(), "C++ should create a valid error result");

            // Cast to our Rust Result type
            let rust_result = &*(cpp_error as *const Result<i32>);

            // Convert to std::result::Result
            let std_result: std::result::Result<i32, Error> =
                std::ptr::read(rust_result).into();

            println!("C++ error -> std::result::Result conversion:");
            println!("  std_result.is_err(): {}", std_result.is_err());

            assert!(std_result.is_err(), "Converted result should be Err");
            let error = std_result.unwrap_err();
            assert_eq!(error.code(), 999, "Error code should match C++ created error");

            // Clean up
            destroy_score_result_int32(cpp_error);
        }
    }

    #[test]
    fn test_cpp_bool_result_to_std_result() {
        unsafe {
            // Test bool success case
            let cpp_bool_success = create_score_result_success_bool(true);
            let rust_bool_result = &*(cpp_bool_success as *const Result<bool>);

            let std_bool_result: std::result::Result<bool, Error> =
                std::ptr::read(rust_bool_result).into();

            assert!(std_bool_result.is_ok(), "Bool success should convert to Ok");
            assert!(std_bool_result.unwrap(), "Bool value should be preserved");

            // Test bool error case
            let test_message = std::ffi::CString::new("Bool error test").unwrap();
            let cpp_bool_error = create_score_result_error_bool(777, test_message.as_ptr());
            let rust_bool_error_result = &*(cpp_bool_error as *const Result<bool>);

            let std_bool_error_result: std::result::Result<bool, Error> =
                std::ptr::read(rust_bool_error_result).into();

            assert!(std_bool_error_result.is_err(), "Bool error should convert to Err");
            assert_eq!(std_bool_error_result.unwrap_err().code(), 777, "Error code should be preserved");

            // Clean up
            destroy_score_result_bool(cpp_bool_success);
            destroy_score_result_bool(cpp_bool_error);
        }
    }

    #[test]
    fn test_multiple_cpp_results_conversion() {
        unsafe {
            let test_message = std::ffi::CString::new("Multiple conversion test").unwrap();

            // Create multiple C++ results
            let cpp_results = [
                (create_score_result_success_int32(100), true, 100),
                (create_score_result_success_int32(200), true, 200),
                (create_score_result_error_int32(300, test_message.as_ptr()), false, 300),
                (create_score_result_error_int32(400, test_message.as_ptr()), false, 400),
            ];

            println!("Testing multiple C++ results conversion:");

            for (i, &(cpp_result_ptr, should_be_ok, expected_value_or_code)) in cpp_results.iter().enumerate() {
                assert!(!cpp_result_ptr.is_null(), "C++ result {} should be valid", i);

                let rust_result = &*(cpp_result_ptr as *const Result<i32>);
                let std_result: std::result::Result<i32, Error> =
                    std::ptr::read(rust_result).into();

                if should_be_ok {
                    assert!(std_result.is_ok(), "Result {} should be Ok", i);
                    assert_eq!(std_result.unwrap(), expected_value_or_code,
                              "Result {} value should match", i);
                    println!("  Result {}: Ok({}) ✓", i, expected_value_or_code);
                } else {
                    assert!(std_result.is_err(), "Result {} should be Err", i);
                    assert_eq!(std_result.unwrap_err().code(), expected_value_or_code,
                              "Result {} error code should match", i);
                    println!("  Result {}: Err(code={}) ✓", i, expected_value_or_code);
                }

                destroy_score_result_int32(cpp_result_ptr);
            }
        }
    }

    #[test]
    fn test_cpp_result_conversion_with_rust_operations() {
        unsafe {
            // Create C++ success result
            let cpp_success = create_score_result_success_int32(50);
            let rust_result = &*(cpp_success as *const Result<i32>);

            // Convert to std::result::Result and perform Rust operations
            let std_result: std::result::Result<i32, Error> =
                std::ptr::read(rust_result).into();

            // Test map operation
            let mapped_result = std_result.map(|x| x * 2);
            assert!(mapped_result.is_ok());
            assert_eq!(mapped_result.unwrap(), 100);

            // Create another C++ result for chaining test
            let cpp_success2 = create_score_result_success_int32(25);
            let rust_result2 = &*(cpp_success2 as *const Result<i32>);
            let std_result2: std::result::Result<i32, Error> =
                std::ptr::read(rust_result2).into();

            // Test and_then operation
            let chained_result = std_result2.and_then(|x| {
                if x > 20 {
                    Ok(x + 10)
                } else {
                    // Create a Rust error for testing
                    let error_str = "Value too small";
                    let c_str = std::ffi::CString::new(error_str).unwrap();
                    let string_view = CStringView::from_parts(c_str.as_ptr(), error_str.len());
                    let error = Error::from_parts(999, ptr::null(), string_view);
                    Err(error)
                }
            });

            assert!(chained_result.is_ok());
            assert_eq!(chained_result.unwrap(), 35);

            println!("C++ result -> std::result conversions with Rust operations: ✓");

            // Clean up
            destroy_score_result_int32(cpp_success);
            destroy_score_result_int32(cpp_success2);
        }
    }

    #[test]
    fn test_cpp_error_result_conversion_with_error_handling() {
        unsafe {
            let test_message = std::ffi::CString::new("Error handling test").unwrap();

            // Create C++ error result
            let cpp_error = create_score_result_error_int32(404, test_message.as_ptr());
            let rust_result = &*(cpp_error as *const Result<i32>);

            // Convert to std::result::Result and test error handling
            let std_result: std::result::Result<i32, Error> =
                std::ptr::read(rust_result).into();

            // Test map_err operation
            let mapped_error_result = std_result.map_err(|err| {
                println!("Mapped error code: {} -> {}", err.code(), err.code() + 1000);
                Error::from_parts(err.code() + 1000, err.domain(), err.message())
            });

            assert!(mapped_error_result.is_err());
            assert_eq!(mapped_error_result.unwrap_err().code(), 1404);

            // Test or_else operation
            let cpp_success_fallback = create_score_result_success_int32(999);
            let rust_fallback_result = &*(cpp_success_fallback as *const Result<i32>);
            let std_fallback_result: std::result::Result<i32, Error> =
                std::ptr::read(rust_fallback_result).into();

            let fallback_result = std_result.or(std_fallback_result);
            assert!(fallback_result.is_ok());
            assert_eq!(fallback_result.unwrap(), 999);

            println!("C++ error result -> std::result error handling operations: ✓");

            // Clean up
            destroy_score_result_int32(cpp_error);
            destroy_score_result_int32(cpp_success_fallback);
        }
    }
}
