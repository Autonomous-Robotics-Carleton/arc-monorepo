import defaultMdxComponents from 'fumadocs-ui/mdx';
import type { MDXComponents } from 'mdx/types';
import { Tab, Tabs } from 'fumadocs-ui/components/tabs';
import { Step, Steps } from 'fumadocs-ui/components/steps';
import { Accordion, Accordions } from 'fumadocs-ui/components/accordion';
import { File, Files, Folder } from 'fumadocs-ui/components/files';
import { TypeTable } from 'fumadocs-ui/components/type-table';
import { Mermaid } from '@/components/Mermaid';

export function getMDXComponents(components?: MDXComponents): MDXComponents {
  return {
    ...defaultMdxComponents,
    // Tabs
    Tab,
    Tabs,
    // Steps
    Step,
    Steps,
    // Accordion
    Accordion,
    Accordions,
    // File tree
    File,
    Files,
    Folder,
    // Type table
    TypeTable,
    // ```mermaid blocks (remarkMdxMermaid in source.config.ts)
    Mermaid,
    ...components,
  };
}
