# Live Handoff / Update Template for kimi-cli

**Use this format when you need to give kimi new information while a task is in progress.**

---

**Task:** [Current task name, e.g. "Port PageCurl to 2-pass"]

**Update Type:** 
- [ ] New context / requirements
- [ ] Bug / issue found
- [ ] Change in direction
- [ ] Additional constraints (Emscripten, performance, etc.)
- [ ] Other: ________________

**Message to kimi:**

```
[Write your update here. Be clear and concise.]

Example:
- The PageCurl shader should keep the original curl geometry in Pass 0
- In Pass 1 we only want to add soft highlights + backface tint
- Use iLastPassTex only in Pass 1
- Keep backward compatibility with single-pass mode
```

**New Instructions / Next Step:**

```
[What kimi should do next]
```

**Priority:** High / Medium / Low

---

**Copy-paste ready version:**

```
**Task:** [Task Name]
**Update Type:** [Type]
**Message:** 
[Your message here]
**Next Step:** 
[What to do next]
**Priority:** [High/Medium/Low]
```