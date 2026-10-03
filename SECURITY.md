# Security Policy

Thank you for helping keep ARC (Autonomous Robotics Carleton) and its documentation secure. This document describes how to report security vulnerabilities, what information to provide, our handling and disclosure process, and other guidelines for contributors and researchers.

---

## 1. Reporting a Vulnerability

If you discover a security issue in this repository or the systems described by its documentation, please report it privately to the maintainers so we can investigate and remediate it before public disclosure.

Preferred contact methods:

- Email: `devlead@arcarleton.ca` or `financeadmin@arcarleton.ca`
- GitHub: **Report a vulnerability** on the repository's Security tab (private vulnerability reporting). Public issues are visible to everyone, so never use them for security reports.

When reporting, please include:

1. A clear description of the vulnerability and its impact.
2. Steps to reproduce (minimum viable reproduction preferred).
3. Proof-of-concept (PoC) code or screenshots where applicable.
4. The version/commit, environment, and any configuration needed to reproduce.
5. Your suggested remediation (optional).
6. How we can reach you for follow-up (email or GPG key).

Do **not** publicly disclose the vulnerability until it has been addressed by maintainers and coordinated disclosure is complete.

---

## 2. Handling and Response Process

We aim to acknowledge receipt of a vulnerability report within **3 business days** and provide an estimated timeline for remediation.

Our triage and remediation process:

1. **Triage** — confirm the issue, classify severity, and determine scope.
2. **Investigation** — reproduce and identify root cause.
3. **Remediation** — implement and test a fix.
4. **Coordinated Disclosure** — communicate with the reporter, release a patched version (or update docs), and publish an advisory.
5. **Credit** — with the reporter's permission, we will credit researchers who responsibly disclose vulnerabilities.

---

## 3. Severity Classification (Guideline)

- **Critical** — remote code execution, authentication bypass, or large-scale data exposure.
- **High** — privilege escalation, significant information disclosure, or broken access control.
- **Medium** — limited information disclosure, predictable errors that can be chained.
- **Low** — minor issues such as insecure headers, minor info leakage, or configuration best-practices.

This classification guides remediation priority but is not exhaustive — maintainers will determine final severity.

---

## 4. Disclosure Timeline

We prefer coordinated disclosure. Our default timeline (subject to change depending on complexity):

- Acknowledgement: within **3 business days**.
- Initial triage and estimated timeline: within **7 calendar days**.
- Patch or mitigation: typically within **90 days** for non-critical issues; for critical issues we will prioritize faster fixes and may release emergency patches.
- Public advisory: after patch rollout or mutual agreement with the reporter.

If you need a different timeline (for example, to meet regulatory or disclosure requirements), please state it in your report.

---

## 5. Safe Harbor & Good Faith

We welcome security researchers and contributors acting in good faith. If you follow this policy and act responsibly (no undue disruption, no data exfiltration beyond what is required to demonstrate the issue), we will not pursue legal action against you for your research.

We ask that you:

- Avoid accessing or modifying private user data beyond what is necessary to demonstrate the issue.
- Avoid introducing persistent changes to production systems.
- Follow applicable laws and your local rules for security research.

---

## 6. How to Submit a Report Securely

If you want to encrypt your report:

1. Provide a PGP key in your initial contact or request the maintainer's PGP key.
2. Use standard encryption best practices and include your contact details so we can respond.

If emailing, include `[SECURITY]` in the subject line.

---

## 7. What We Will Provide

- Timely updates during triage and remediation.
- A CVE assignment where appropriate (we will request one if needed).
- An advisory and patched release (or a clear mitigation) once the issue is fixed.
- Optional recognition in our `SECURITY.md` or `AUTHORS.md` when the reporter consents.

---

## 8. Scope

This policy applies to the `arc-monorepo` repository: its documentation, websites, scripts, CI workflows and the car software and firmware hosted within it.

Exclusions (out of scope):

- Third-party services and dependencies not maintained by the ARC project (report to the upstream maintainers or disclose per their policies).
- Issues affecting infrastructure outside the repository unless the documentation directly causes misuse.

If you’re unsure whether something is in scope, send us the report privately and we will triage it.

---

## 9. Contact & Maintainer Information

Maintainership and contact details should be kept up to date in the repo. Suggested canonical contact:

- `devlead@arcarleton.ca`
- `financeadmin@arcarleton.ca`
- GitHub: private vulnerability reporting on the Security tab.

---

## 10. Acknowledgements

We appreciate the time and effort of security researchers who responsibly disclose issues. With permission, we will credit reporters in release notes or an acknowledgements section.

---

## 11. Revision History

- 2025-09-18 — Initial security policy created for ARC Docs.
- 2026-10-03 — Renamed repository (arc-monorepo), widened scope to the car code, reporting through GitHub private vulnerability reporting.

---

*Last updated: 2026-10-03*
