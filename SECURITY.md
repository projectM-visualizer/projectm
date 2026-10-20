# Security Policy

## Supported Versions

Use this section to tell people about which versions of your project are
currently being supported with security updates.

| Version | Supported          |
| ------- | ------------------ |
| 5.1.x   | :white_check_mark: |
| 5.0.x   | :x:                |
| 4.0.x   | :white_check_mark: |
| < 4.0   | :x:                |

## Reporting a Vulnerability

Use this section to tell people how to report a vulnerability.
Title:
Stack-based buffer overrun in projectm_pcm_add_float when count > projectm_pcm_get_max_samples()
Description:
In projectM-4, calling projectm_pcm_add_float() (and projectm_pcm_add_int16()) with a count larger than projectm_pcm_get_max_samples() (512) 
causes a stack-based buffer overrun inside projectm-4.dll.
On Windows (MSVC builds with /GS), this triggers:
Unhandled exception in projectM-4.dll: Stack cookie instrumentation code detected a stack-based buffer overrun (0xC0000409)
followed by an access violation at 0xC0000005 reading offset +0x2B due to corrupted local pointers on the stack.
Steps to Reproduce:
code
C++
projectm_handle pm = projectm_create();
projectm_set_window_size(pm, 800, 600);

// Feed a standard 1024-frame stereo buffer (1024 > max_samples of 512)
std::vector<float> pcm(2048, 0.5f);
projectm_pcm_add_float(pm, pcm.data(), 1024, PROJECTM_STEREO); // CRASH / Stack cookie violation
Root Cause:
projectm_pcm_add_float copies count samples per channel into an internal buffer sized for 512 samples without clamping count 
to the buffer limit.
Suggested Fix:
In projectm_pcm_add_float (and integer counterparts), validate and clamp count before copying:
code
C
unsigned int max_samples = projectm_pcm_get_max_samples();
if (count > max_samples) {
    count = max_samples; // Clamp to prevent stack buffer overrun
}
Additionally, calling projectm_opengl_render_frame() before any preset has been loaded can trigger an access violation
dereferencing active_preset (reading offset +0x2B). A null-check guard for an active preset before rendering frames would resolve 
that as well.


----------------------------------------------------------------

x64 there no crash
x86  Exception thrown at 0x67B8E6F0 (projectM-4.dll) in Token.exe: 0xC0000005: Access violation reading location 0x0000002B. 
Unhandled exception at 0x67BD8725 (projectM-4.dll) in Token.exe: Stack cookie instrumentation code detected a stack-based buffer overrun. 
Unhandled exception at 0x67BD8725 (projectM-4.dll) 
---------------------------------------------------------------------


Does this mean projectM has a vulnerability?
Yes. In security terms, this is classified as CWE-121: Stack-based Buffer Overflow and CWE-120: Buffer Copy without Checking Size of Input.
Why it is a vulnerability:
Lack of Input Validation: Even though projectM provides an API function called projectm_pcm_get_max_samples() (which returns 512),
the function projectm_pcm_add_float(instance, data, count, channels) accepts count as an unsigned integer from the caller and writes
that many samples into an internal fixed-size stack buffer without checking if count <= 512.
Impact (Denial of Service / Memory Corruption): When an application passes more than 512 frames (such as standard audio chunks of 1024 or 2048 
samples), projectM-4.dll smashes its own stack frame. This overwrites the MSVC /GS security cookie and corrupts surrounding memory
(causing the 0x0000002B access violation and triggering STATUS_STACK_BUFFER_OVERRUN).
Defense-in-Depth Failure: A C shared library (.dll / .so) should never trust caller-provided size parameters blindly.
If an application or plugin passes a size larger than the internal buffer, the library must clamp the input 
or discard excess frames—never overrun the stack.
-------------------------------------------------------------------------------------------------

